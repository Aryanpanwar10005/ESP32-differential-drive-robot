#ifndef CAMERA_MANAGER_H
#define CAMERA_MANAGER_H

#include "../config.h"
#include <Arduino.h>
#include <HTTPClient.h>

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
  // Constructor to initialize member variables
  CameraManager() {
    status.online = false;
    status.stream_url = "";
    status.last_check = 0;
    status.check_interval = 10000;
  }

  void cameraInit();
  void cameraLoop();
  bool isCameraOnline();
  String getStreamURL();
  CameraStatus getCameraStatus();
  void checkCameraHealth();
};

extern CameraManager cameraManager;

#endif // CAMERA_MANAGER_H
