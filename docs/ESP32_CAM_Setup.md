# Camera Streaming (PDF Mandatory Requirement)

## Hardware

-   ESP32-CAM (AI-Thinker)
-   OV2640 camera sensor
-   PSRAM required for VGA resolution

## Streaming Method

-   MJPEG over HTTP
-   Endpoint: `/stream`
-   Port: 81
-   Frame rate: ~8 FPS

## Security

-   Camera stream inaccessible until BLE authentication succeeds
-   403 Forbidden response before auth

## Access Instructions

1. Power ESP32-CAM
2. Authenticate via BLE
3. Open browser: `http://<ESP32-IP>:81/stream`

## Acceptance Test Coverage

✔ Continuous live video stream  
✔ BLE-gated access enforced  
✔ <600ms latency achievable  
✔ Wi-Fi streaming operational  
✔ Non-blocking firmware design

## Troubleshooting

-   **"Camera init failed"**: Check power supply (5V stable), verify board seating
-   **"No video"**: Verify BLE auth completed, check firewall port 81
-   **"Choppy video"**: Reduce frame size in `camera_manager.cpp`, improve Wi-Fi signal
