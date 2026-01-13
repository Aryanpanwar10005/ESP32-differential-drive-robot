#ifndef BLE_AUTH_H
#define BLE_AUTH_H

#include "../config.h"
#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

class BleAuthCallbacks; // Forward declaration

class BleAuth {
private:
  bool authenticated = false;
  bool bleActive = true;
  BLEServer *pServer = nullptr;
  BLECharacteristic *pAuthCharacteristic = nullptr;

  friend class BleAuthCallbacks; // Allow callback access to private members

public:
  void bleAuthInit(String deviceName);
  bool isAuthenticated();
  void resetAuthentication();
  void disableBLE();
};

extern BleAuth bleAuth;

#endif // BLE_AUTH_H
