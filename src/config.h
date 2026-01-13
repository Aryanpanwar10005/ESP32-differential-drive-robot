#ifndef CONFIG_H
#define CONFIG_H

// ===== FIRMWARE METADATA =====
#define FIRMWARE_VERSION    "1.0.0"
#define BUILD_DATE          "2026-01-13"

// ===== MOTOR CONTROL (TB6612FNG) =====
#define LEFT_PWM     25
#define LEFT_IN1     26
#define LEFT_IN2     27
#define RIGHT_PWM    32
#define RIGHT_IN1    33
#define RIGHT_IN2    14
#define MOTOR_STBY   15

// ===== PWM CONFIGURATION =====
#define PWM_FREQ            5000  // 5kHz
#define PWM_RES             8     // 8-bit resolution
#define PWM_CHANNEL_LEFT    0
#define PWM_CHANNEL_RIGHT   1
#define MAX_SPEED           200   // Cap at 200/255

// ===== SERVO CONTROL =====
#define SERVO_PIN           13
#define SERVO_MIN_ANGLE     10
#define SERVO_MAX_ANGLE     170
#define SERVO_CENTER_ANGLE  90

// ===== GPS MODULE =====
#define GPS_RX_PIN          16
#define GPS_TX_PIN          17
#define GPS_BAUD            9600
#define GPS_UPDATE_INTERVAL 5000

// ===== BLE AUTHENTICATION =====
#define AUTH_TOKEN          "SecureToken123"
#define BLE_DEVICE_NAME     "ESP32_Robot_001"
#define BLE_SERVICE_UUID    "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define BLE_AUTH_CHAR_UUID  "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// ===== NETWORK CONFIGURATION =====
#define WIFI_SSID               "YourWiFiSSID"
#define WIFI_PASSWORD           "YourWiFiPassword"
#define WEBSOCKET_SERVER_URL    "ws://192.168.1.100:8080/ws"
#define BOT_ID                  "BOT_001"
#define TELEMETRY_INTERVAL      2000  // ms
#define NETWORK_RECONNECT_BASE  1000  // ms

// ===== ESP32-CAM CONFIGURATION =====
#define ESP32_CAM_IP            "192.168.1.50"
#define ESP32_CAM_STREAM_URL    "http://192.168.1.50/stream"

// ===== STATUS LED =====
#define LED_PIN                 2
#define LED_BLINK_INTERVAL      500   // WAIT_AUTH state
#define LED_BLINK_ERROR         250   // ERROR state

// ===== SERIAL DEBUGGING =====
#define SERIAL_BAUD         115200

#endif // CONFIG_H
