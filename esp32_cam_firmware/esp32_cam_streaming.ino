/*
 * ESP32-CAM MJPEG Streaming Firmware v1.0
 * Hardware: AI-Thinker ESP32-CAM + OV2640
 */

#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>

// Camera pins (AI-Thinker ESP32-CAM)
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

// WiFi (MUST MATCH MAIN ESP32)
const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";
IPAddress local_IP(192, 168, 1, 50);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== ESP32-CAM Streaming v1.0 ===");
  
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_VGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("[ERROR] Camera init: 0x%x\n", err);
    while(1);
  }
  Serial.println("[CAMERA] Initialized");
  
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("[WARN] Static IP failed");
  }
  
  WiFi.begin(ssid, password);
  Serial.print("[WIFI] Connecting");
  
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - start > 15000) {
      Serial.println("\n[ERROR] WiFi timeout");
      while(1);
    }
    delay(500);
    Serial.print(".");
  }
  
  Serial.printf("\n[WIFI] IP: %s\n", WiFi.localIP().toString().c_str());
  
  server.on("/", HTTP_GET, handleRoot);
  server.on("/stream", HTTP_GET, handleStream);
  server.on("/status", HTTP_GET, handleStatus);
  server.onNotFound([]() { server.send(404, "text/plain", "Not Found"); });
  
  server.begin();
  Serial.printf("[STREAM] http://%s/stream\n", WiFi.localIP().toString().c_str());
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  String html = "<html><body><h1>ESP32-CAM</h1>";
  html += "<img src='/stream' style='max-width:100%'></body></html>";
  server.send(200, "text/html", html);
}

void handleStream() {
  WiFiClient client = server.client();
  
  client.println("HTTP/1.1 200 OK");
  client.printf("Content-Type: %s\r\n", _STREAM_CONTENT_TYPE);
  client.println("Connection: close\r\n");
  
  while (client.connected()) {
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) break;
    
    client.print(_STREAM_BOUNDARY);
    client.printf(_STREAM_PART, fb->len);
    client.write(fb->buf, fb->len);
    esp_camera_fb_return(fb);
    
    if (!client.connected()) break;
  }
}

void handleStatus() {
  String json = "{\"status\":\"online\",\"ip\":\"";
  json += WiFi.localIP().toString() + "\"}";
  server.send(200, "application/json", json);
}
