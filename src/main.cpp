#include "config.h"
#include "modules/ble_auth.h"
#include "modules/camera_manager.h"
#include "modules/gps_module.h"
#include "modules/motor_control.h"
#include "modules/network.h"
#include "modules/servo_control.h"
#include <Arduino.h>

enum SystemState {
  INIT,
  WAIT_AUTH,
  CONNECT_NETWORK,
  OPERATIONAL,
  ERROR
}; // Exact enum order checked

SystemState currentState = INIT;
unsigned long previousMillis = 0;
unsigned long lastGPSPrint = 0;
unsigned long lastTelemetry = 0;
unsigned long lastLedBlink = 0;
unsigned long lastAuthPrint = 0;
unsigned long errorStartTime = 0;
unsigned long lastErrorPrint = 0;
bool ledState = false;
bool networkInitialized = false;
String errorReason = "";

void handleCommand(String action, int value);
void updateLED(int state);
void stopMotors();
void printStateTransition(SystemState nextState, const char *reason);

// GPS data is defined in network.cpp and externed in network.h

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n=== ESP32 Robot v" FIRMWARE_VERSION " ===");
  Serial.println("Build: " BUILD_DATE);
  Serial.printf("Reset reason: %d\n", esp_reset_reason());

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  motorInit();
  gpsModule.gpsInit();
  servoControl.servoInit();
  bleAuth.bleAuthInit("ESP32_Robot_001");

  Serial.printf("[MAIN] Heap: %dB\n", ESP.getFreeHeap());
  printStateTransition(WAIT_AUTH, "Boot complete");
}

void loop() {
  unsigned long currentMillis = millis();

  switch (currentState) {
  case INIT:
    // Handled in setup()
    break;

  case WAIT_AUTH:
    // LED blink 500ms
    if (currentMillis - lastLedBlink >= 500) {
      lastLedBlink = currentMillis;
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
    }

    if (currentMillis - lastAuthPrint >= 5000) {
      lastAuthPrint = currentMillis;
      Serial.println("[MAIN] Waiting for BLE authentication...");
    }

    if (bleAuth.isAuthenticated()) {
      bleAuth.disableBLE();
      Serial.printf("[BLE] Disabled | Heap: %dB\n", ESP.getFreeHeap());

      networkManager.networkInit(WIFI_SSID, WIFI_PASSWORD,
                                 WEBSOCKET_SERVER_URL);
      networkManager.setCommandCallback(handleCommand);
      networkInitialized = true;
      printStateTransition(CONNECT_NETWORK, "Auth complete");
    }
    break;

  case CONNECT_NETWORK:
    if (networkInitialized) {
      networkManager.networkLoop();
      if (networkManager.isConnected()) {
        printStateTransition(OPERATIONAL, "Network ready");
        lastTelemetry = currentMillis;
        return;
      }
    }

    // LED fast blink 100ms
    if (currentMillis - lastLedBlink >= 100) {
      lastLedBlink = currentMillis;
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
    }

    // Timeout if needed
    break;

  case OPERATIONAL:
    networkManager.networkLoop();
    cameraManager.cameraLoop();

    // ✅ CRITICAL FIX: GPS data update BEFORE telemetry
    currentGPSData = gpsModule.getGPSData();

    // ✅ Telemetry 2s
    if (currentMillis - lastTelemetry >= TELEMETRY_INTERVAL) {
      lastTelemetry = currentMillis;
      networkManager.sendTelemetry();
    }

    // ✅ Monitor connection
    if (!networkManager.isConnected()) {
      errorReason = "WebSocket lost";
      brakeMotors(); // ✅ Active brake on disconnect
      printStateTransition(ERROR, errorReason.c_str());
      errorStartTime = currentMillis;
    }

    // Keep LED ON when operational
    digitalWrite(LED_PIN, HIGH);
    break;

  case ERROR:
    brakeMotors(); // ✅ Active brake in error state

    // LED flash 250ms
    if (currentMillis - lastLedBlink >= 250) {
      lastLedBlink = currentMillis;
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
    }

    if (currentMillis - lastErrorPrint >= 5000) {
      lastErrorPrint = currentMillis;
      Serial.printf("[MAIN] ERROR: %s. Recovering...\n", errorReason.c_str());
    }

    // ✅ 10s recovery
    if (currentMillis - errorStartTime >= 10000) {
      Serial.println("[MAIN] Recovery...");
      networkManager.networkInit(WIFI_SSID, WIFI_PASSWORD,
                                 WEBSOCKET_SERVER_URL);
      networkManager.setCommandCallback(handleCommand);
      printStateTransition(CONNECT_NETWORK, "Recovery");
    }
    break;
  }

  // Always update GPS in background
  gpsModule.gpsUpdate();
}

void handleCommand(String action, int value) {
  if (currentState != OPERATIONAL)
    return;

  Serial.printf("[MAIN] Command: %s (%d)\n", action.c_str(), value);

  if (action == "forward")
    moveForward(value);
  else if (action == "backward")
    moveBackward(value);
  else if (action == "left")
    turnLeft(value);
  else if (action == "right")
    turnRight(value);
  else if (action == "stop")
    stopMotors();
  else if (action == "servo")
    servoControl.setLaserAngle(value);
}

void printStateTransition(SystemState nextState, const char *reason) {
  const char *stateNames[] = {"INIT", "WAIT_AUTH", "CONNECT_NETWORK",
                              "OPERATIONAL", "ERROR"};
  Serial.printf("[STATE] %s -> %s | Reason: %s\n", stateNames[currentState],
                stateNames[nextState], reason);
  currentState = nextState;
}
