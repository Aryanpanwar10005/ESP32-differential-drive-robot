#ifndef CAMERA_MANAGER_H
#define CAMERA_MANAGER_H

#include <Arduino.h>

#if defined(USE_ESP32_CAM)
// Full implementation for ESP32-CAM hardware
#include "esp_camera.h"
#include <WiFi.h>

class CameraManager {
public:
  CameraManager();
  bool init();
  void startServer(uint16_t port = 81);
  void loop();
  void stop();
  void setAuthState(bool authenticated);

private:
  bool cameraInitialized;
  bool serverRunning;
  bool bleAuthenticated;
  WiFiClient streamClient;
  bool streamActive;
  unsigned long lastFrameMillis;
};

#else
// Stub implementation for non-camera builds (esp32dev, tests)
class CameraManager {
public:
  CameraManager() {}

  bool init() {
    Serial.println(
        "[Camera] Camera disabled in this build (no USE_ESP32_CAM flag)");
    return false;
  }

  void startServer(uint16_t port = 81) {
    Serial.println("[Camera] Camera not available in this build");
  }

  void loop() {}
  void stop() {}
  void setAuthState(bool authenticated) {}
};
#endif

#endif // CAMERA_MANAGER_H
