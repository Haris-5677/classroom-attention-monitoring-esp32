//==============================================================
// Camera Pin Definitions for ESP32-CAM (AI-Thinker Model)
//
// This file is required by the sketch to tell the compiler which
// GPIO pins are used for the camera interface on this specific board.
//==============================================================

#if defined(CAMERA_MODEL_AI_THINKER)

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1 // The reset pin is not connected on the standard AI-Thinker
#define XCLK_GPIO_NUM     0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM       5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#define LED_GPIO_NUM      33  // Status LED on some boards
#define FLASH_GPIO_NUM    4   // Flash LED (White LED)

#endif