#ifndef CAMERA_MANAGER_H
#define CAMERA_MANAGER_H

#include <Arduino.h>
#include <HTTPClient.h>
#include "config.h"

struct CameraStatus {
    bool online;
    String stream_url;
    unsigned long last_check;
    int check_interval;
};

class CameraManager {
private:
    CameraStatus status;
    HTTPClient http;
    
public:
    void cameraInit();
    void cameraLoop();
    bool isCameraOnline();
    String getStreamURL();
    CameraStatus getCameraStatus();
    void checkCameraHealth();
};

extern CameraManager cameraManager;

#endif // CAMERA_MANAGER_H
