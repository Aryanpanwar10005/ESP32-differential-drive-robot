#ifndef GPS_MODULE_H
#define GPS_MODULE_H

#include "../config.h"
#include <Arduino.h>
#include <TinyGPS++.h>

struct GPSData {
  float lat;
  float lon;
  bool fix_valid;
  uint32_t timestamp;
};

class GPSModule {
private:
  HardwareSerial gpsSerial;
  TinyGPSPlus gps;
  unsigned long lastUpdate;
  unsigned long lastSend;
  GPSData currentData;

public:
  GPSModule();
  void gpsInit();
  void gpsUpdate();
  bool shouldSendGPS();
  String getGPSJSON();
  GPSData getGPSData();

  // Getters for test access
  int getSatellites();
  double getHDOP();
  uint32_t getCharsProcessed();
};

extern GPSModule gpsModule;

#endif // GPS_MODULE_H
