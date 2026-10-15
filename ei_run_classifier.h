#include "esp_camera.h"
#include "WiFi.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "soc/soc.h" // Disable brownout issues
#include "soc/rtc_cntl_reg.h"

// ------------------------------------------
// 1. YOUR EDGE IMPULSE MODEL LIBRARY IMPORT
// ------------------------------------------
// !!! IMPORTANT: YOU MUST ADD YOUR .H FILE HERE !!!
#include "ei_run_classifier.h" 
// This file name must match the main header in your Edge Impulse library export.

// ------------------------------------------
// 2. WIFI CONFIGURATION (UPDATE THESE)
// ------------------------------------------
const char* ssid = "Haris"; // <-- YOUR WIFI SSID
const char* password = "fushiguro"; // <-- YOUR WIFI PASSWORD

// ------------------------------------------
// 3. PIN DEFINITIONS (Standard AI-Thinker Pinout)
// ------------------------------------------
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
#define LED_GPIO_NUM      33

// The labels from your Edge Impulse project
const char *EI_LABELS[] = {
    "label1", // E.g., "cat"
    "label2", // E.g., "dog"
    "label3"  // E.g., "empty"
    // Check your model's generated header file for the correct list
};

// ------------------------------------------
// 4. CAMERA CONFIGURATION (POWER OPTIMIZED)
// ------------------------------------------
static camera_config_t camera_config = {
    .pin_pwdn  = PWDN_GPIO_NUM,
    .pin_reset = RESET_GPIO_NUM,
    .pin_xclk  = XCLK_GPIO_NUM,
    .pin_sscb_sda = SIOD_GPIO_NUM,
    .pin_sscb_scl = SIOC_GPIO_NUM,
    .pin_d7 = Y9_GPIO_NUM,
    .pin_d6 = Y8_GPIO_NUM,
    .pin_d5 = Y7_GPIO_NUM,
    .pin_d4 = Y6_GPIO_NUM,
    .pin_d3 = Y5_GPIO_NUM,
    .pin_d2 = Y4_GPIO_NUM,
    .pin_d1 = Y3_GPIO_NUM,
    .pin_d0 = Y2_GPIO_NUM,
    .pin_vsync = VSYNC_GPIO_NUM,
    .pin_href  = HREF_GPIO_NUM,
    .pin_pclk  = PCLK_GPIO_NUM,

    .xclk_freq_hz = 10000000, 
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,
    
    // IMPORTANT: Your Edge Impulse model expects a specific format (e.g., RGB565)
    // You MUST set the correct format here, not JPEG, if your model requires RGB/Grayscale
    .pixel_format = PIXFORMAT_RGB565, 
    
    // Set framesize to match your EI input resolution (e.g., 96x96 is roughly LLD)
    // Check the exact FRAMESIZE_... that matches your model's resolution!
    .frame_size = FRAMESIZE_96X96, 
    
    .jpeg_quality = 12,
    .fb_count = 1, // CRITICAL: Only one frame buffer
    .fb_location = CAMERA_FB_IN_PSRAM,
    .grab_mode = CAMERA_GRAB_LATEST,
};

// ------------------------------------------
// 5. DATA ACQUISITION FUNCTION FOR EI
// ------------------------------------------
// This function is required by the Edge Impulse library to get image data.
// It reads from the global frame buffer captured in the loop.
static camera_fb_t *g_fb = NULL;
int ei_camera_get_data(size_t offset, size_t length, float *out_ptr) {
    if (offset + length > g_fb->len) {
        Serial.printf("Ran out of data! Offset: %d, Length: %d, FB Length: %d\n", offset, length, g_fb->len);
        return 1;
    }
    
    // The image data is stored in g_fb->buf. We need to convert it to floating-point
    // and potentially adjust the color space (e.g., RGB565 to Grayscale or RGB)
    // Edge Impulse's internal library often handles this conversion based on the 
    // format specified in the generated model.

    // Assuming your model is 96x96 RGB:
    size_t pixel_offset = offset / 3; // For 3 color channels
    
    for (size_t i = 0; i < length; i++) {
        // Simple 8-bit to float conversion
        out_ptr[i] = (float)g_fb->buf[offset + i] / 255.0f;
    }

    return 0;
}


// ------------------------------------------
// 6. SETUP FUNCTION
// ------------------------------------------
void setup() {
    // Disable brownout detection (risky, but can prevent unexpected resets)
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 
    
    Serial.begin(115200);
    while(!Serial) { delay(10); } 
    Serial.println("\n--- ESP32-CAM TinyML Inference ---");

    // Phase 1: Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi Connected! IP: " + WiFi.localIP().toString());
    
    // Phase 2: Initialize Camera
    Serial.println("Attempting Camera Initialization...");
    esp_err_t err = esp_camera_init(&camera_config);

    if (err != ESP_OK) {
        Serial.printf("Camera init FAILED! Error: 0x%x. Restarting in 5s.\n", err);
        delay(5000);
        ESP.restart(); // Restart if camera fails
        return;
    }
    Serial.println("Camera initialized successfully!");
    
    // Phase 3: Check EI model size
    Serial.printf("Edge Impulse Model Input Size: %dx%d\n", EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT);
}

// ------------------------------------------
// 7. LOOP FUNCTION (Inference Logic)
// ------------------------------------------
void loop() {
    // 1. Capture Frame
    g_fb = esp_camera_fb_get();
    if (!g_fb) {
        Serial.println("Camera capture failed!");
        delay(1000);
        return;
    }

    Serial.printf("\nCaptured Frame: Size %d bytes, Format: %d\n", g_fb->len, g_fb->format);

    // 2. Run Edge Impulse Inference
    ei_impulse_result_t result = { 0 };
    signal_t signal;
    
    // Configure signal properties
    signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT * EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME;
    signal.get_data = &ei_camera_get_data;

    long inference_start = millis();
    EI_CLASSIFIER_ERROR res = run_classifier(&signal, &result, false);
    long inference_time = millis() - inference_start;

    // 3. Print Results
    if (res != EI_CLASSIFIER_OK) {
        Serial.printf("Classifier failed with error: %d\n", res);
        esp_camera_fb_return(g_fb);
        delay(1000);
        return;
    }

    Serial.printf("[EI Result] Inference time: %ld ms\n", inference_time);
    
    // Find the best prediction
    float max_confidence = 0;
    const char *predicted_label = "None";

    for (size_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        if (result.classification[i].value > max_confidence) {
            max_confidence = result.classification[i].value;
            predicted_label = EI_LABELS[i];
        }
        Serial.printf("  %s: %.2f%%\n", EI_LABELS[i], result.classification[i].value * 100.0f);
    }
    
    Serial.printf("--> PREDICTION: %s (%.2f%% confidence)\n", predicted_label, max_confidence * 100.0f);

    // 4. Cleanup and Delay
    esp_camera_fb_return(g_fb);
    delay(2000); 
}