#include "camera_manager.h"

#if defined(USE_ESP32_CAM)

#include <WebServer.h>

static WebServer cameraServer(81);

CameraManager::CameraManager()
    : cameraInitialized(false), serverRunning(false), bleAuthenticated(false),
      streamActive(false), lastFrameMillis(0) {}

bool CameraManager::init() {
  camera_config_t config;

  // AI-Thinker ESP32-CAM pin map
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;

  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 12;
    config.fb_count = 2;
    Serial.println("[Camera] PSRAM found, using VGA");
  } else {
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 15;
    config.fb_count = 1;
    Serial.println("[Camera] No PSRAM, using QVGA");
  }

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("[Camera] Init failed");
    return false;
  }

  cameraInitialized = true;
  Serial.println("[Camera] ESP32-CAM initialized");
  return true;
}

void CameraManager::setAuthState(bool authenticated) {
  bleAuthenticated = authenticated;
  if (authenticated) {
    Serial.println("[Camera] BLE auth granted - stream enabled");
  }
}

void CameraManager::startServer(uint16_t port) {
  if (!cameraInitialized) {
    Serial.println("[Camera] Cannot start - not initialized");
    return;
  }

  cameraServer.on("/", HTTP_GET, [&]() {
    if (!bleAuthenticated) {
      cameraServer.send(
          403, "text/html",
          "<h1>403 Forbidden</h1><p>BLE authentication required</p>");
      return;
    }

    String html = "<!DOCTYPE html><html><body>";
    html += "<h1>ESP32 Robot Camera</h1>";
    html += "<img src='/stream' width='640'>";
    html += "</body></html>";
    cameraServer.send(200, "text/html", html);
  });

  cameraServer.on("/stream", HTTP_GET, [&]() {
    if (!bleAuthenticated) {
      cameraServer.send(403, "text/plain", "BLE authentication required");
      Serial.println("[Camera] Stream denied - no BLE auth");
      return;
    }

    if (streamActive) {
      cameraServer.send(503, "text/plain", "Stream already active");
      return;
    }

    streamClient = cameraServer.client();
    streamClient.println("HTTP/1.1 200 OK");
    streamClient.println(
        "Content-Type: multipart/x-mixed-replace; boundary=frame");
    streamClient.println();

    streamActive = true;
    lastFrameMillis = 0;

    Serial.println("[Camera] Stream started");
  });

  cameraServer.begin(port);
  serverRunning = true;

  Serial.printf("[Camera] Server started on port %d\n", port);
  Serial.printf("[Camera] Access: http://<ESP32-IP>:%d/stream\n", port);
}

void CameraManager::loop() {
  if (!serverRunning)
    return;

  cameraServer.handleClient();

  if (streamActive) {
    if (!streamClient.connected()) {
      streamClient.stop();
      streamActive = false;
      Serial.println("[Camera] Stream ended");
      return;
    }

    unsigned long now = millis();
    if (now - lastFrameMillis < 120)
      return;

    lastFrameMillis = now;

    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("[Camera] Frame capture failed");
      return;
    }

    streamClient.printf(
        "--frame\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n",
        fb->len);
    streamClient.write(fb->buf, fb->len);
    streamClient.print("\r\n");

    esp_camera_fb_return(fb);
  }
}

void CameraManager::stop() {
  if (streamActive) {
    streamClient.stop();
    streamActive = false;
  }

  if (serverRunning) {
    cameraServer.stop();
    serverRunning = false;
  }

  Serial.println("[Camera] Stopped");
}

#endif // USE_ESP32_CAM
