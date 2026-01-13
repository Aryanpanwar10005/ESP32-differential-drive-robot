#define FIRMWARE_VERSION "v1.0.0"
#define BUILD_DATE __DATE__ " " __TIME__

// Pin definitions
#define LEFT_PWM 25
#define LEFT_IN1 26
#define LEFT_IN2 27
#define RIGHT_PWM 32
#define RIGHT_IN1 33
#define RIGHT_IN2 14
#define MOTOR_STBY 15
#define GPS_RX 16
#define GPS_TX 17
#define SERVO_PIN 13
#define LED_PIN 2
#define SERIAL_BAUD 115200

// Config
#define WIFI_SSID "YourNetwork"
#define WIFI_PASSWORD "YourPassword"
#define WEBSOCKET_SERVER_URL "ws://192.168.1.100:8080/ws"
#define BLE_DEVICE_NAME "ESP32_Robot_001"
#define BOT_ID "robot_001"
#define ESP32_CAM_STREAM_URL "http://192.168.1.50/stream"
#define GPS_UPDATE_INTERVAL 5000
#define TELEMETRY_INTERVAL 2000
#define LED_BLINK_INTERVAL 500
#define LED_BLINK_ERROR 250
#define MAX_SPEED 255
#define SERVO_MIN_ANGLE 10
#define SERVO_MAX_ANGLE 170
#define NETWORK_RECONNECT_BASE 5000

#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
#include <TinyGPSPlus.h>

// ===== MODULES =====
Servo laserServo;
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

// Motor control
void motorInit() {
  pinMode(LEFT_PWM, OUTPUT);
  pinMode(LEFT_IN1, OUTPUT);
  pinMode(LEFT_IN2, OUTPUT);
  pinMode(RIGHT_PWM, OUTPUT);
  pinMode(RIGHT_IN1, OUTPUT);
  pinMode(RIGHT_IN2, OUTPUT);
  pinMode(MOTOR_STBY, OUTPUT);
  digitalWrite(MOTOR_STBY, HIGH);
}

void moveForward(int speed) {
  analogWrite(LEFT_PWM, speed);
  digitalWrite(LEFT_IN1, HIGH);
  digitalWrite(LEFT_IN2, LOW);
  analogWrite(RIGHT_PWM, speed);
  digitalWrite(RIGHT_IN1, HIGH);
  digitalWrite(RIGHT_IN2, LOW);
}

void moveBackward(int speed) {
  analogWrite(LEFT_PWM, speed);
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, HIGH);
  analogWrite(RIGHT_PWM, speed);
  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, HIGH);
}

void moveLeft(int speed) {
  analogWrite(LEFT_PWM, speed/2);
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, HIGH);
  analogWrite(RIGHT_PWM, speed/2);
  digitalWrite(RIGHT_IN1, HIGH);
  digitalWrite(RIGHT_IN2, LOW);
}

void moveRight(int speed) {
  analogWrite(LEFT_PWM, speed/2);
  digitalWrite(LEFT_IN1, HIGH);
  digitalWrite(LEFT_IN2, LOW);
  analogWrite(RIGHT_PWM, speed/2);
  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, HIGH);
}

void stopMotors() {
  analogWrite(LEFT_PWM, 0);
  analogWrite(RIGHT_PWM, 0);
  digitalWrite(LEFT_IN1, LOW);
  digitalWrite(LEFT_IN2, LOW);
  digitalWrite(RIGHT_IN1, LOW);
  digitalWrite(RIGHT_IN2, LOW);
}

// GPS
struct GPSData {
  double latitude;
  double longitude;
  bool fix_valid;
  unsigned long timestamp;
};

GPSData currentGPSData = {0, 0, false, 0};

String getGPSJSON() {
  DynamicJsonDocument doc(256);
  doc["lat"] = currentGPSData.latitude;
  doc["lon"] = currentGPSData.longitude;
  doc["fix"] = currentGPSData.fix_valid;
  String json;
  serializeJson(doc, json);
  return json;
}

void gpsInit() {
  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
}

void gpsUpdate() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        currentGPSData.latitude = gps.location.lat();
        currentGPSData.longitude = gps.location.lng();
        currentGPSData.fix_valid = true;
        currentGPSData.timestamp = millis();
      }
    }
  }
}

GPSData getGPSData() {
  return currentGPSData;
}

// Servo
void servoInit() {
  laserServo.attach(SERVO_PIN);
  laserServo.write(90);
}

void setLaserAngle(int angle) {
  laserServo.write(angle);
}

// Network
WebSocketsClient webSocket;
enum NetworkState { NETWORK_DISCONNECTED, NETWORK_CONNECTING, NETWORK_CONNECTED };
NetworkState state = NETWORK_DISCONNECTED;
unsigned long lastTelemetry = 0;
unsigned long lastReconnectAttempt = 0;
unsigned long reconnectDelay = NETWORK_RECONNECT_BASE;
void (*commandCallback)(String, int) = nullptr;

void onWebSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("[NETWORK] WebSocket DISCONNECTED");
      state = NETWORK_DISCONNECTED;
      stopMotors();
      break;
    case WStype_CONNECTED:
      Serial.printf("[NETWORK] WebSocket CONNECTED\n");
      state = NETWORK_CONNECTED;
      reconnectDelay = NETWORK_RECONNECT_BASE;
      break;
    case WStype_TEXT:
      {
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, payload);
        if (doc["type"] == "control") {
          String action = doc["action"];
          int value = doc["speed"] | doc["angle"] | 0;
          if (commandCallback) commandCallback(action, value);
        }
      }
      break;
  }
}

void networkInit(String ssid, String password, String serverURL) {
  Serial.printf("[NETWORK] Connecting WiFi: %s\n", ssid.c_str());
  WiFi.begin(ssid.c_str(), password.c_str());
  
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
    delay(100);
    Serial.print(".");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[NETWORK] WiFi OK: " + WiFi.localIP().toString());
    webSocket.begin(serverURL.c_str());
    webSocket.onEvent(onWebSocketEvent);
    webSocket.setReconnectInterval(5000);
  } else {
    Serial.println("\n[NETWORK] WiFi timeout");
  }
}

void networkLoop() {
  webSocket.loop();
  if (state == NETWORK_DISCONNECTED && millis() - lastReconnectAttempt > reconnectDelay) {
    webSocket.begin(WEBSOCKET_SERVER_URL);
    lastReconnectAttempt = millis();
    reconnectDelay = min(reconnectDelay * 2, 8000UL);
  }
  if (state == NETWORK_CONNECTED && millis() - lastTelemetry >= TELEMETRY_INTERVAL) {
    sendTelemetry();
    lastTelemetry = millis();
  }
}

bool isConnected() { return state == NETWORK_CONNECTED; }

void sendTelemetry() {
  DynamicJsonDocument doc(512);
  doc["type"] = "telemetry";
  doc["bot_id"] = BOT_ID;
  doc["gps"]["lat"] = currentGPSData.latitude;
  doc["gps"]["lon"] = currentGPSData.longitude;
  doc["gps"]["fix"] = currentGPSData.fix_valid;
  doc["camera_url"] = ESP32_CAM_STREAM_URL;
  doc["uptime"] = millis();
  String json;
  serializeJson(doc, json);
  webSocket.sendTXT(json);
}

void setCommandCallback(void (*cb)(String, int)) {
  commandCallback = cb;
}

// BLE Auth (simplified)
bool isAuthenticated = false;

bool bleAuthInit(String name) {
  Serial.println("[BLE] Auth simulation enabled");
  return true;
}

bool isAuthenticated() { return isAuthenticated; }
void disableBLE() { isAuthenticated = true; }

// ===== MAIN STATE MACHINE =====
enum SystemState { INIT, WAIT_AUTH, CONNECT_NETWORK, OPERATIONAL, ERROR };
SystemState currentState = INIT;
unsigned long lastGPSPrint = 0, lastLedBlink = 0, lastAuthPrint = 0, errorStartTime = 0;
bool ledState = false;
String errorReason = "";

void handleCommand(String action, int value);

void setup() {
  Serial.begin(SERIAL_BAUD);
  pinMode(LED_PIN, OUTPUT);
  
  Serial.printf("=== ESP32 Robot %s (%s) ===\n", FIRMWARE_VERSION, BUILD_DATE);
  
  motorInit();
  gpsInit();
  servoInit();
  bleAuthInit(BLE_DEVICE_NAME);
  
  currentState = WAIT_AUTH;
  Serial.println("[MAIN] → WAIT_AUTH");
}

void loop() {
  unsigned long now = millis();
  
  gpsUpdate();
  if (now - lastGPSPrint >= GPS_UPDATE_INTERVAL && currentGPSData.fix_valid) {
    lastGPSPrint = now;
    Serial.println("[GPS] Fix: " + getGPSJSON());
  }
  
  switch (currentState) {
    case WAIT_AUTH:
      if (now - lastLedBlink >= LED_BLINK_INTERVAL) {
        lastLedBlink = now;
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
      }
      if (now - lastAuthPrint >= GPS_UPDATE_INTERVAL) {
        lastAuthPrint = now;
        Serial.println("[MAIN] Waiting BLE auth...");
      }
      if (isAuthenticated()) {
        Serial.println("[BLE] Auth OK!");
        digitalWrite(LED_PIN, HIGH);
        disableBLE();
        networkInit(WIFI_SSID, WIFI_PASSWORD, WEBSOCKET_SERVER_URL);
        setCommandCallback(handleCommand);
        currentState = CONNECT_NETWORK;
        Serial.println("[MAIN] → CONNECT_NETWORK");
      }
      break;
      
    case CONNECT_NETWORK:
      networkLoop();
      if (isConnected()) {
        currentState = OPERATIONAL;
        Serial.println("[MAIN] → OPERATIONAL");
      } else if (now - lastLedBlink > 30000) {
        errorReason = "Network timeout";
        currentState = ERROR;
        errorStartTime = now;
      }
      break;
      
    case OPERATIONAL:
      networkLoop();
      currentGPSData = getGPSData();
      if (!isConnected()) {
        errorReason = "WebSocket lost";
        currentState = ERROR;
        errorStartTime = now;
      }
      break;
      
    case ERROR:
      stopMotors();
      if (now - lastLedBlink >= LED_BLINK_ERROR) {
        lastLedBlink = now;
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
      }
      if (now - errorStartTime >= 10000) {
        Serial.println("[MAIN] Recovery...");
        networkInit(WIFI_SSID, WIFI_PASSWORD, WEBSOCKET_SERVER_URL);
        currentState = CONNECT_NETWORK;
      }
      break;
  }
}

void handleCommand(String action, int value) {
  if (currentState != OPERATIONAL) return;
  
  Serial.printf("[CMD] %s(%d)\n", action.c_str(), value);
  value = constrain(value, 0, MAX_SPEED);
  
  if (action == "forward") moveForward(value);
  else if (action == "backward") moveBackward(value);
  else if (action == "left") moveLeft(value);
  else if (action == "right") moveRight(value);
  else if (action == "stop") stopMotors();
  else if (action == "servo") setLaserAngle(constrain(value, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE));
}
