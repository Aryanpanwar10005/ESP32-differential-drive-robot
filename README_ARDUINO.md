# ESP32 Robot Firmware - Arduino IDE Version

> ⚠️ **Note:** This is a **legacy single-file version** for Arduino IDE. For production deployment, use the **PlatformIO version** with modular architecture (see main [README.md](README.md)).

## Overview

Simplified all-in-one firmware for ESP32 differential drive robot. Includes motor control, GPS tracking, servo control, BLE authentication, and WebSocket communication in a single `.ino` file.

## Hardware Requirements

- ESP32-WROOM-32 development board
- TB6612FNG motor driver
- 2x DC motors (6V)
- NEO-6M GPS module
- SG90 servo motor
- LED on GPIO 2
- 7.4V battery + buck converters

## Pin Configuration

| Component       | ESP32 Pin |
| --------------- | --------- |
| Left Motor PWM  | GPIO 25   |
| Left Motor IN1  | GPIO 26   |
| Left Motor IN2  | GPIO 27   |
| Right Motor PWM | GPIO 32   |
| Right Motor IN1 | GPIO 33   |
| Right Motor IN2 | GPIO 14   |
| Motor STBY      | GPIO 15   |
| GPS RX          | GPIO 16   |
| GPS TX          | GPIO 17   |
| Servo           | GPIO 13   |
| Status LED      | GPIO 2    |

## Arduino IDE Setup

### 1. Install ESP32 Board Support

1. Open Arduino IDE
2. Go to **File → Preferences**
3. Add to "Additional Boards Manager URLs":
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Go to **Tools → Board → Boards Manager**
5. Search "ESP32" and install "esp32 by Espressif Systems"

### 2. Install Required Libraries

Go to **Tools → Manage Libraries** and install:

- `ESP32Servo` by Kevin Harrington (v1.2.1+)
- `TinyGPSPlus` by Mikal Hart (v1.0.3+)
- `ArduinoWebsockets` by Gil Maimon (v0.5.3+)
- `ArduinoJson` by Benoit Blanchon (v7.0.0+)

### 3. Configure Board Settings

- **Board:** ESP32 Dev Module
- **Upload Speed:** 921600
- **CPU Frequency:** 240MHz
- **Flash Frequency:** 80MHz
- **Flash Size:** 4MB
- **Partition Scheme:** Default 4MB with spiffs
- **Core Debug Level:** None (for production)
- **Port:** Select your ESP32's COM port

## Configuration

Before uploading, edit these lines in `ESP32_Robot.ino`:

```cpp
// WiFi credentials (REQUIRED)
#define WIFI_SSID "YourNetwork"        // Change this
#define WIFI_PASSWORD "YourPassword"    // Change this

// WebSocket server (REQUIRED)
#define WEBSOCKET_SERVER_URL "ws://192.168.1.100:8080/ws"  // Your server IP

// Robot identification
#define BOT_ID "robot_001"              // Unique robot ID

// ESP32-CAM stream URL (if using camera)
#define ESP32_CAM_STREAM_URL "http://192.168.1.50/stream"
```

## Upload Instructions

1. Connect ESP32 via USB
2. Select correct Port in Tools menu
3. Click Upload button (→)
4. Wait for "Hard resetting via RTS pin..." message
5. Open Serial Monitor (115200 baud)

## Expected Serial Output

```
=== ESP32 Robot v1.0.0 ===
[MOTOR] Initialized
[GPS] UART configured
[SERVO] Initialized at 90°
[BLE] Auth simulation enabled
[MAIN] → WAIT_AUTH
[MAIN] Waiting BLE auth...
[BLE] Auth OK!
[NETWORK] Connecting WiFi: YourNetwork
[NETWORK] WiFi OK: 192.168.1.123
[MAIN] → CONNECT_NETWORK
[NETWORK] WebSocket CONNECTED
[MAIN] → OPERATIONAL
[GPS] Fix: {"lat":28.6139,"lon":77.2090,"fix":true}
```

## State Machine

The firmware operates in 5 states:

1. **INIT** → Hardware initialization
2. **WAIT_AUTH** → LED blinks (500ms), waiting for BLE auth
3. **CONNECT_NETWORK** → Connecting WiFi + WebSocket
4. **OPERATIONAL** → Normal operation, sending telemetry
5. **ERROR** → LED fast blink (250ms), auto-recovery after 10s

## Control Commands (JSON)

### Motor Control

```json
{"type": "control", "action": "forward", "speed": 150}
{"type": "control", "action": "backward", "speed": 150}
{"type": "control", "action": "left", "speed": 100}
{"type": "control", "action": "right", "speed": 100}
{"type": "control", "action": "stop"}
```

### Servo Control

```json
{ "type": "control", "action": "servo", "angle": 90 }
```

## Telemetry Output (JSON)

Sent every 2 seconds when operational:

```json
{
  "type": "telemetry",
  "bot_id": "robot_001",
  "gps": {
    "lat": 28.613939,
    "lon": 77.209021,
    "fix": true
  },
  "camera_url": "http://192.168.1.50/stream",
  "uptime": 123456
}
```

## Troubleshooting

| Issue                 | Solution                                                 |
| --------------------- | -------------------------------------------------------- |
| Compilation errors    | Install all 4 required libraries                         |
| Upload fails          | Check COM port, press BOOT button during upload          |
| WiFi won't connect    | Verify SSID/password, check 2.4GHz network               |
| Motors don't move     | Check wiring, verify STBY pin HIGH, test battery voltage |
| No GPS fix            | Move outdoors, wait 30-60s for cold start                |
| WebSocket disconnects | Verify server URL and port, check firewall               |

## Limitations vs PlatformIO Version

This Arduino IDE version has some limitations:

- ❌ No BLE authentication (simulated only)
- ❌ No camera manager module
- ❌ No unit tests
- ❌ Single-file architecture (harder to maintain)
- ❌ Uses analogWrite() instead of LEDC API
- ✅ Easier for beginners
- ✅ Quick testing and prototyping

## Migration to PlatformIO

For production use, migrate to the modular PlatformIO version:

```bash
# Clone repository
git clone https://github.com/Aryanpanwar10005/ESP32-differential-drive-robot.git
cd ESP32-differential-drive-robot

# Build and upload
pio run -t upload
pio device monitor
```

See main [README.md](README.md) for complete instructions.

## License

MIT License - See [LICENSE](LICENSE) file

## Support

For issues or questions, open an issue on GitHub:
https://github.com/Aryanpanwar10005/ESP32-differential-drive-robot/issues
