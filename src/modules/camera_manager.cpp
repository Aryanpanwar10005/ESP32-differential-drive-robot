#include "camera_manager.h"

CameraManager cameraManager;

void CameraManager::cameraInit() {
    Serial.println("[CAMERA] Initializing camera manager");
    
    status.online = false;
    status.stream_url = String("http://") + ESP32_CAM_IP + "/stream";
    status.last_check = 0;
    status.check_interval = 10000; // Check every 10s
    
    Serial.printf("[CAMERA] Stream URL: %s\n", status.stream_url.c_str());
}

void CameraManager::cameraLoop() {
    unsigned long now = millis();
    
    // Periodic health check
    if (now - status.last_check >= status.check_interval) {
        status.last_check = now;
        checkCameraHealth();
    }
}

void CameraManager::checkCameraHealth() {
    // Check if ESP32-CAM is responding
    String healthURL = String("http://") + ESP32_CAM_IP + "/";
    
    http.setTimeout(2000); // 2s timeout
    http.begin(healthURL);
    
    int httpCode = http.GET();
    
    if (httpCode > 0) {
        status.online = true;
        Serial.println("[CAMERA] ESP32-CAM online");
    } else {
        status.online = false;
        Serial.println("[CAMERA] ESP32-CAM unreachable");
    }
    
    http.end();
}

bool CameraManager::isCameraOnline() {
    return status.online;
}

String CameraManager::getStreamURL() {
    return status.stream_url;
}

CameraStatus CameraManager::getCameraStatus() {
    return status;
}
