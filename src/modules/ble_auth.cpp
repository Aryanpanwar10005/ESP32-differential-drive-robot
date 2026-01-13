#include "ble_auth.h"
#include "config.h"
#include "esp_bt.h"
#include "esp_bt_main.h" // For esp_bluedroid_disable
#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

BleAuth bleAuth;

class BleAuthCallbacks : public BLECharacteristicCallbacks {
public:
  void onWrite(BLECharacteristic *pCharacteristic) override {
    std::string rxValue = pCharacteristic->getValue();
    Serial.printf("[BLE] Token: %s\n", rxValue.c_str());

    if (rxValue == AUTH_TOKEN) {
      Serial.println("[BLE] Authentication successful!");
      bleAuth.authenticated = true;
      bleAuth.pAuthCharacteristic->setValue("AUTH_OK");
      bleAuth.pServer->getAdvertising()->stop();
      Serial.println("[BLE] Advertising stopped");
    } else {
      Serial.println("[BLE] Auth failed!");
      bleAuth.pAuthCharacteristic->setValue("AUTH_FAIL");
    }
    bleAuth.pAuthCharacteristic->notify();
  }
};

void BleAuth::bleAuthInit(String deviceName) {
  Serial.printf("[BLE] Init: %s\n", deviceName.c_str());

  BLEDevice::init(deviceName.c_str());
  Serial.printf("[BLE] MAC: %s\n", BLEDevice::getAddress().toString().c_str());

  pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(BLE_SERVICE_UUID);

  pAuthCharacteristic = pService->createCharacteristic(
      BLE_AUTH_CHAR_UUID, BLECharacteristic::PROPERTY_READ |
                              BLECharacteristic::PROPERTY_WRITE |
                              BLECharacteristic::PROPERTY_NOTIFY);

  pAuthCharacteristic->setCallbacks(new BleAuthCallbacks());
  pAuthCharacteristic->setValue("READY");
  pAuthCharacteristic->addDescriptor(new BLE2902());

  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(BLE_SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();

  bleActive = true;
  Serial.printf("[BLE] Ready! Write '%s' to auth\n", AUTH_TOKEN);
}

bool BleAuth::isAuthenticated() { return authenticated; }

void BleAuth::resetAuthentication() {
  authenticated = false;
  Serial.println("[BLE] Auth reset");
}

void BleAuth::disableBLE() {
  static bool disabled = false;
  if (disabled)
    return;

  if (bleActive) {
    pServer->getAdvertising()->stop();
    esp_bluedroid_disable();
    esp_bt_controller_disable();
    bleActive = false;
    disabled = true;
    Serial.println("[BLE] Stack disabled, ~60KB RAM freed");
    Serial.printf("[BLE] Heap after: %dB\n", ESP.getFreeHeap());
  }
}
