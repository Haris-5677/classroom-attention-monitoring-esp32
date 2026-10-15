# AI-Driven Classroom Distraction Detection Using ESP32-CAM

An embedded AI mini project that uses the ESP32-CAM and TinyML
to identify potential classroom distraction based on head position.

The system captures images using the ESP32-CAM, processes them
through a trained machine learning model, and classifies the
observed behavior as either looking up or looking down.

The goal is to explore how low-cost embedded hardware and
on-device AI can be used for classroom attention monitoring.

## Project Overview

Maintaining attention in a classroom can be challenging.
This project explores a computer-vision-based approach to
identifying potential distraction using head-position cues.

An AI model developed and deployed using Edge Impulse performs
image inference on the ESP32-CAM platform. The system processes
captured image data and produces a classification result.

The inference process runs locally on the device, avoiding the
need to send images to a cloud-based AI service.

## Objectives

- Develop a low-cost embedded AI prototype for classroom
  attention monitoring.
- Use ESP32-CAM for image acquisition.
- Apply TinyML for image classification on embedded hardware.
- Distinguish between looking-up and looking-down head positions.
- Explore real-time inference on resource-constrained devices.
- Demonstrate an offline computer-vision application.

## Hardware Requirements

- AI-Thinker ESP32-CAM development board
- OV2640 camera module
- USB-to-serial programmer for uploading firmware
- Suitable power supply
- Jumper wires

## Software and Technologies

- Arduino IDE
- ESP32 Arduino Core
- Edge Impulse
- TinyML
- Computer Vision
- Embedded AI
- Serial Monitor for displaying classification results

## System Workflow

1. **Image Capture:** The OV2640 camera captures an image.
2. **Image Processing:** The captured image is prepared for
   inference using the configured image-processing pipeline.
3. **Model Inference:** The trained Edge Impulse model analyzes
   the image on the ESP32-CAM.
4. **Classification:** The model identifies the predicted
   head-position category.
5. **Result Output:** The classification result and confidence
   information are displayed through serial output.

## Classification Labels

The model uses two primary categories:

| Label | Interpretation |
|---|---|
| Looking Up | Not Distracted |
| Looking Down | Potentially Distracted |

The classification is based on visible head-position cues.
Looking down does not necessarily mean that a student is
distracted, and looking up does not guarantee attention.

## Model and Inference

The project uses an Edge Impulse machine-learning workflow
for deploying image classification on embedded hardware.

The implementation uses a confidence threshold of 0.60
(60%) for interpreting predictions.

The image-processing and inference configuration includes
96 × 96 grayscale input for the model.

The camera captures QVGA JPEG images before the configured
image-processing pipeline prepares the input for inference.

> Note: The camera capture resolution and model input
> dimensions serve different purposes. The image-processing
> pipeline prepares the captured image for the model.

## Key Features

- ESP32-CAM-based image acquisition
- Embedded machine-learning inference
- Head-position-based classification
- Edge Impulse model deployment
- Local inference without mandatory cloud processing
- Serial output for observing prediction results
- Low-cost hardware suitable for prototyping

## Applications

- Educational technology prototypes
- Classroom attention-monitoring research
- Embedded computer-vision experiments
- TinyML demonstrations
- AI-based behavioral cue classification

## Limitations

- Head position alone cannot reliably determine a person's
  attention or mental state.
- Looking down may indicate reading, writing, or taking notes.
- Lighting, camera angle, image quality, and occlusion can
  affect model predictions.
- Model accuracy depends on the quality and diversity of
  the training dataset.
- This prototype should not be treated as a definitive
  measure of student attention or used as the sole basis
  for evaluating students.

## Privacy Considerations

Classroom monitoring involves sensitive visual information.
Any real-world deployment should consider informed consent,
institutional policies, data minimization, and applicable
privacy requirements.

The project is designed around local inference rather than
mandatory cloud-based image processing.

## Future Improvements

- Expand the dataset with diverse head positions and lighting
  conditions.
- Evaluate model performance on unseen images.
- Add additional head-position categories.
- Improve robustness under different camera angles.
- Explore suitable visual or audio feedback mechanisms.
- Evaluate inference latency and resource utilization.

## Project Status

Developed as an academic mini project to explore TinyML,
embedded computer vision, and AI-assisted classroom
attention monitoring.

## Author
* **Mohammed Haris**
* LinkedIn: [linkedin.com/in/mohammed-haris-34a805409](https://www.linkedin.com/in/mohammed-haris-34a805409)
* GitHub: [github.com/Haris-5677](https://github.com/Haris-5677)
