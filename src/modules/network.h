#ifndef NETWORK_H
#define NETWORK_H
#include <Arduino.h>

#include "../config.h"
#include "gps_module.h"
#include <ArduinoJson.h>
#include <ArduinoWebsockets.h>
#include <WiFi.h>

extern GPSData currentGPSData;

enum NetworkState {
  NETWORK_DISCONNECTED,
  NETWORK_CONNECTING,
  NETWORK_CONNECTED
};

class NetworkManager {
private:
  websockets::WebsocketsClient webSocket;
  NetworkState state = NETWORK_DISCONNECTED;
  unsigned long lastReconnectAttempt = 0;
  unsigned long reconnectDelay = NETWORK_RECONNECT_BASE;
  unsigned long lastTelemetry = 0;
  void (*commandCallback)(String action, int value) = nullptr;

  void onWebSocketEvent(websockets::WebsocketsEvent event, String data);
  void onWebSocketMessage(websockets::WebsocketsMessage message);
  void attemptReconnect();

public:
  bool networkInit(String ssid, String password, String serverURL);
  void networkLoop();
  bool isConnected();
  void sendTelemetry();
  void setCommandCallback(void (*callback)(String action, int value));
};

extern NetworkManager networkManager; // Renamed to avoid namespace conflict

#endif // NETWORK_H
