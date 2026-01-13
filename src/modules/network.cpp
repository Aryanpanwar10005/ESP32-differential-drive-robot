#include "network.h"
#include "ble_auth.h"
#include "camera_manager.h"
#include "gps_module.h"
#include "motor_control.h"

NetworkManager networkManager; // Renamed to avoid namespace conflict
GPSData currentGPSData;

void NetworkManager::onWebSocketEvent(websockets::WebsocketsEvent event,
                                      String data) {
  switch (event) {
  case websockets::WebsocketsEvent::ConnectionOpened:
    Serial.printf("[NETWORK] WebSocket CONNECTED\n");
    state = NETWORK_CONNECTED;
    reconnectDelay = NETWORK_RECONNECT_BASE;
    break;

  case websockets::WebsocketsEvent::ConnectionClosed:
    Serial.printf("[NETWORK] WebSocket DISCONNECTED\n");
    state = NETWORK_DISCONNECTED;
    stopMotors();
    break;

  case websockets::WebsocketsEvent::GotPing:
    Serial.println("[NETWORK] Got ping");
    break;

  case websockets::WebsocketsEvent::GotPong:
    Serial.println("[NETWORK] Got pong");
    break;
  }
}

void NetworkManager::onWebSocketMessage(websockets::WebsocketsMessage message) {
  Serial.printf("[NETWORK] Received: %s\n", message.data().c_str());

  // Parse JSON command
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, message.data());

  if (error) {
    Serial.printf("[NETWORK] JSON parse error: %s\n", error.c_str());
    return;
  }

  const char *msgType = doc["type"];
  if (String(msgType) == "control") {
    String action = doc["action"];
    // Get value from speed field, or angle field if speed not present
    int value = 0;
    if (doc.containsKey("speed")) {
      value = doc["speed"].as<int>();
    } else if (doc.containsKey("angle")) {
      value = doc["angle"].as<int>();
    }

    if (commandCallback) {
      commandCallback(action, value);
    }
  }
}

bool NetworkManager::networkInit(String ssid, String password,
                                 String serverURL) {
  Serial.printf("[NETWORK] Connecting to WiFi: %s\n", ssid.c_str());

  WiFi.begin(ssid.c_str(), password.c_str());

  // Non-blocking 10s timeout (100ms intervals)
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 100) {
    delay(100);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[NETWORK] WiFi connected! IP: " +
                   WiFi.localIP().toString());
    state = NETWORK_CONNECTING;

    // Setup callbacks
    webSocket.onEvent([this](websockets::WebsocketsEvent event, String data) {
      this->onWebSocketEvent(event, data);
    });
    webSocket.onMessage([this](websockets::WebsocketsMessage message) {
      this->onWebSocketMessage(message);
    });

    // Connect to WebSocket server
    webSocket.connect(serverURL);
    return true;
  } else {
    Serial.println("\n[NETWORK] WiFi connection failed! (10s timeout)");
    // Ensure motors remain stopped on connection failure
    stopMotors();
    return false;
  }
}

void NetworkManager::networkLoop() {
  webSocket.poll();

  // Reconnect logic
  if (state == NETWORK_DISCONNECTED &&
      millis() - lastReconnectAttempt > reconnectDelay) {
    attemptReconnect();
  }

  // Telemetry every 2s when connected
  if (state == NETWORK_CONNECTED &&
      millis() - lastTelemetry >= TELEMETRY_INTERVAL) {
    sendTelemetry();
    lastTelemetry = millis();
  }
}

bool NetworkManager::isConnected() { return state == NETWORK_CONNECTED; }

void NetworkManager::sendTelemetry() {
  JsonDocument doc;
  GPSData data = gpsModule.getGPSData();

  doc["type"] = "telemetry";
  doc["bot_id"] = BOT_ID;
  doc["authenticated"] = bleAuth.isAuthenticated();
  doc["gps"]["lat"] = data.lat;
  doc["gps"]["lon"] = data.lon;
  doc["gps"]["fix"] = data.fix_valid;

  // Camera status
  doc["camera"]["online"] = cameraManager.isCameraOnline();
  doc["camera"]["stream_url"] = cameraManager.getStreamURL();

  doc["camera_url"] = ESP32_CAM_STREAM_URL;
  doc["uptime"] = millis();
  doc["free_heap"] = ESP.getFreeHeap();

  String telemetry;
  serializeJson(doc, telemetry);

  if (webSocket.available()) {
    webSocket.send(telemetry);
    Serial.println("[NETWORK] Telemetry sent (" + String(telemetry.length()) +
                   " bytes)");
  }
}

void NetworkManager::setCommandCallback(void (*callback)(String action,
                                                         int value)) {
  commandCallback = callback;
}

void NetworkManager::attemptReconnect() {
  Serial.printf("[NETWORK] Reconnecting in %lu ms...\n", reconnectDelay);
  lastReconnectAttempt = millis();

  if (WiFi.status() == WL_CONNECTED) {
    webSocket.connect(WEBSOCKET_SERVER_URL);
    state = NETWORK_CONNECTING;
  }

  // Exponential backoff
  reconnectDelay = min(reconnectDelay * 2, 8000UL);
}
