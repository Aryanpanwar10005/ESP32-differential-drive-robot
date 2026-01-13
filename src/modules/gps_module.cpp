#include "gps_module.h"

GPSModule gpsModule;

GPSModule::GPSModule()
    : gpsSerial(2), lastUpdate(0), lastSend(0), lastValidFixMillis(0) {
  currentData = {0.0f, 0.0f, false, 0};
}

void GPSModule::gpsInit() {
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  Serial.println(
      "[GPS] GPS module initialized on UART2 (RX=16, TX=17, 9600 baud)");
}

void GPSModule::gpsUpdate() {
  // Non-blocking GPS data parsing (buffer < 512 bytes)
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      // Update GPS data when valid fix available
      if (gps.location.isValid()) {
        currentData.lat = gps.location.lat();
        currentData.lon = gps.location.lng();
        currentData.fix_valid = true;
        currentData.lastUpdateMillis = millis();
      } else {
        currentData.fix_valid = false;
      }
    }
  }
}

bool GPSModule::shouldSendGPS() {
  unsigned long now = millis();
  if (now - lastSend >= GPS_UPDATE_INTERVAL) {
    lastSend = now;
    return true;
  }
  return false;
}

String GPSModule::getGPSJSON() {
  String json = "{";
  json += "\"lat\":" + String(currentData.lat, 6) + ",";
  json += "\"lon\":" + String(currentData.lon, 6) + ",";
  json += "\"fix\":" + String(currentData.fix_valid ? "true" : "false");
  json += "}";
  return json;
}

GPSData GPSModule::getGPSData() {
  GPSData data;

  // Check if GPS library has new data
  if (gps.location.isUpdated() && gps.location.isValid()) {
    lastValidFixMillis = millis(); // Update ONLY when GPS updates
  }

  // Build current data
  if (gps.location.isValid()) {
    data.lat = gps.location.lat();
    data.lon = gps.location.lng();
    data.lastUpdateMillis = lastValidFixMillis;

    // Expire fix if last update was >5 seconds ago
    if (millis() - lastValidFixMillis > 5000) {
      data.fix_valid = false; // Stale fix
    } else {
      data.fix_valid = true; // Fresh fix
    }
  } else {
    data.lat = 0.0;
    data.lon = 0.0;
    data.fix_valid = false;
    data.lastUpdateMillis = 0;
  }

  return data;
}

// Getters for test access
int GPSModule::getSatellites() { return gps.satellites.value(); }

double GPSModule::getHDOP() { return gps.hdop.hdop(); }

uint32_t GPSModule::getCharsProcessed() { return gps.charsProcessed(); }
