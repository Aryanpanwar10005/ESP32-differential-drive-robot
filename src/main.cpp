#include <Arduino.h>
#include "config.h"
#include "modules/motor_control.h"
#include "modules/gps_module.h"
#include "modules/servo_control.h"
#include "modules/ble_auth.h"
#include "modules/network.h"
#include "modules/camera_manager.h"

enum SystemState { 
    INIT,
    WAIT_AUTH,
    CONNECT_NETWORK,
    OPERATIONAL,
    ERROR
}; // ✅ EXACT ENUM ORDER ✓

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

const char* stateToString(SystemState state) {
    switch(state) {
        case INIT: return "INIT";
        case WAIT_AUTH: return "WAIT_AUTH";
        case CONNECT_NETWORK: return "CONNECT_NETWORK";
        case OPERATIONAL: return "OPERATIONAL";
        case ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

void printStateTransition(SystemState newState, const char* reason = "") {
    Serial.printf("[MAIN] → %s (%s) | Heap: %dB\n", 
                  stateToString(newState), reason, ESP.getFreeHeap());
}

void setup() {
    Serial.begin(SERIAL_BAUD);
    while (!Serial) { ; }
    
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    
    Serial.printf("=== ESP32 Robot v%s (%s) ===\n", FIRMWARE_VERSION, BUILD_DATE);
    Serial.printf("[INIT] Free heap: %dB\n", ESP.getFreeHeap());
    
    // ✅ EXACT INIT SEQUENCE
    motorInit();
    gpsModule.gpsInit();
    servoControl.servoInit();
    bleAuth.bleAuthInit(BLE_DEVICE_NAME);
    cameraManager.cameraInit();
    
    printStateTransition(WAIT_AUTH, "Modules initialized");
    lastGPSPrint = millis();
    lastAuthPrint = millis();
    lastLedBlink = millis();
}

void loop() {
    unsigned long currentMillis = millis();
    
    // ✅ GPS every loop (non-blocking)
    gpsModule.gpsUpdate();
    
    // ✅ GPS status every 5s
    if (currentMillis - lastGPSPrint >= GPS_UPDATE_INTERVAL) {
        lastGPSPrint = currentMillis;
        if (gpsModule.getGPSData().fix_valid) {
            Serial.println("[GPS] Fix: " + gpsModule.getGPSJSON());
        }
    }
    
    switch (currentState) {
        case WAIT_AUTH:
            // ✅ 500ms LED blink
            if (currentMillis - lastLedBlink >= LED_BLINK_INTERVAL) {
                lastLedBlink = currentMillis;
                ledState = !ledState;
                digitalWrite(LED_PIN, ledState ? HIGH : LOW);
            }
            
            // ✅ Status every 5s
            if (currentMillis - lastAuthPrint >= GPS_UPDATE_INTERVAL) {
                lastAuthPrint = currentMillis;
                Serial.println("[MAIN] Waiting BLE auth...");
            }
            
            // ✅ Auth check
            if (bleAuth.isAuthenticated()) {
                Serial.println("[BLE] Auth OK!");
                digitalWrite(LED_PIN, HIGH);
                bleAuth.disableBLE();
                Serial.printf("[BLE] Disabled | Heap: %dB\n", ESP.getFreeHeap());
                
                networkManager.networkInit(WIFI_SSID, WIFI_PASSWORD, WEBSOCKET_SERVER_URL);
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
                // ✅ 30s timeout
                if (currentMillis - lastLedBlink > 30000) {
                    errorReason = "Network timeout";
                    stopMotors();
                    printStateTransition(ERROR, errorReason.c_str());
                    errorStartTime = currentMillis;
                }
            }
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
                stopMotors();
                printStateTransition(ERROR, errorReason.c_str());
                errorStartTime = currentMillis;
            }
            break;
            
        case ERROR:
            stopMotors(); // ✅ Safety on every loop
            
            // ✅ 250ms fast blink
            if (currentMillis - lastLedBlink >= LED_BLINK_ERROR) {
                lastLedBlink = currentMillis;
                ledState = !ledState;
                digitalWrite(LED_PIN, ledState ? HIGH : LOW);
            }
            
            // ✅ Status every 5s
            if (currentMillis - lastErrorPrint >= GPS_UPDATE_INTERVAL) {
                lastErrorPrint = currentMillis;
                Serial.printf("[ERROR] %s | Recovery: %ds\n", 
                             errorReason.c_str(), 
                             (10000 - (currentMillis - errorStartTime)) / 1000);
            }
            
            // ✅ 10s recovery
            if (currentMillis - errorStartTime >= 10000) {
                Serial.println("[MAIN] Recovery...");
                networkManager.networkInit(WIFI_SSID, WIFI_PASSWORD, WEBSOCKET_SERVER_URL);
                networkManager.setCommandCallback(handleCommand);
                printStateTransition(CONNECT_NETWORK, "Recovery");
            }
            break;
    }
}

// ✅ EXACT COMMAND HANDLER SPEC
void handleCommand(String action, int value) {
    if (!bleAuth.isAuthenticated()) {
        Serial.println("[SECURITY] Unauthorized!");
        return;
    }
    
    if (currentState != OPERATIONAL) {
        Serial.println("[MAIN] Not operational");
        return;
    }
    
    Serial.printf("[CMD] %s(%d)\n", action.c_str(), value);
    value = constrain(value, 0, MAX_SPEED);
    
    if (action == "forward") moveForward(value);
    else if (action == "backward") moveBackward(value);
    else if (action == "left") turnLeft(value);    // Fixed: matches motor_control.h
    else if (action == "right") turnRight(value);  // Fixed: matches motor_control.h
    else if (action == "stop") stopMotors();
    else if (action == "servo") {
        value = constrain(value, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
        servoControl.setLaserAngle(value);
    }
    else Serial.printf("[MAIN] Unknown: %s\n", action.c_str());
}
