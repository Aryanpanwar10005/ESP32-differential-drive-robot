# ESP32-CAM Integration - Complete Implementation

## ✅ Project Includes Full Camera Streaming

This project contains complete working camera streaming code:

- **Main Firmware:** `src/modules/camera_manager.h/cpp` - Monitors ESP32-CAM
- **CAM Firmware:** `esp32_cam_firmware/esp32_cam_streaming.ino` - MJPEG streaming

### Quick Start

1. Upload main firmware to ESP32-WROOM-32
2. Upload `esp32_cam_streaming.ino` to ESP32-CAM module
3. Both connect to same WiFi network
4. Stream available at `http://192.168.1.50/stream`
5. Main ESP32 reports camera status in telemetry

---

# ESP32-CAM Integration Guide
## 🚀 SEPARATE MODULE ARCHITECTURE (No Code Changes to Main ESP32)

**Main ESP32-WROOM-32**: Motors + GPS + Servo + BLE + Network  
**ESP32-CAM**: Video streaming ONLY (standalone firmware)

```
Main ESP32 ── WiFi ── Server ── Web Dashboard
     ↓                    ↑
ESP32-CAM ── WiFi ──────┘ (telemetry: camera_url)
```

## 📋 Hardware Specifications

| **Feature** | **ESP32-CAM (AI-Thinker)** |
|-------------|----------------------------|
| **Processor** | ESP32-D0WD-V3 dual-core |
| **Camera** | OV2640 2MP (1600x1200@15fps) [2][4] |
| **RAM** | 520KB internal + 8MB PSRAM [1][3] |
| **Flash** | 32Mbit SPI |
| **Wi-Fi** | 802.11 b/g/n (17dBm max) [1] |
| **Power** | **5V input required** (camera needs 5V) [3] |
| **Size** | 27×40.5×4.5mm [1][3] |
| **Antenna** | PCB (2dBi) or U.FL external |

## ⚙️ Firmware Setup (Arduino IDE - NOT PlatformIO)

### 1. **Arduino IDE Configuration**
```
Board: "AI Thinker ESP32-CAM" 
PSRAM: "Enabled" 
Partition Scheme: "Huge APP (3MB No OTA)"
CPU Frequency: 240MHz
Flash Frequency: 80MHz
Flash Mode: QIO
```

### 2. **Install ESP32 Board Package**
```
File → Preferences → Additional Boards Manager URLs:
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
Tools → Board → Boards Manager → "esp32" by Espressif
```

### 3. **CameraWebServer Example**
```
File → Examples → ESP32 → Camera → CameraWebServer
```
**Required Changes:**
```cpp
#define CAMERA_MODEL_AI_THINKER  // Line ~70
const char* ssid = "YourWiFiSSID";     // Same as main ESP32
const char* password = "YourWiFiPassword";
```

### 4. **Upload (FTDI Required)**
```
ESP32-CAM Wiring:
VCC → 5V (NOT 3.3V!)
GND → GND
GPIO0 → GND (programming mode)
U0TX → FTDI RX
U0RX → FTDI TX
```
**Upload Process:**
1. Hold GPIO0 to GND
2. Power on → Release GPIO0 after "Connecting..." 
3. Upload → Remove GPIO0 jumper → Reset

## 🎥 Low-Latency Settings (<600ms E2E)

| **Parameter** | **Value** | **Notes** |
|---------------|-----------|-----------|
| `framesize` | `FRAMESIZE_SVGA` (800x600) | Balance quality/speed |
| `jpeg_quality` | `10` | 0=worst, 63=best (lower=faster) |
| `fb_count` | `2` | Double buffering |
| `brightness` | `0` | Auto |
| `contrast` | `0` | Default |

**Stream URL:** `http://ESP32-CAM-IP/stream`

## 🔌 Main ESP32 Integration

### config.h Updates:
```cpp
#define ESP32_CAM_IP        "192.168.1.50"      // Update after CAM boots
#define ESP32_CAM_STREAM_URL "http://192.168.1.50/stream"
```

### network.cpp sendTelemetry() Update:
```cpp
doc["camera_url"] = ESP32_CAM_STREAM_URL;
```

**Server receives telemetry → Displays stream in dashboard**

## 🚀 Deployment Workflow

```
1. Flash ESP32-CAM (Arduino IDE CameraWebServer)
2. Power ESP32-CAM → Note IP from Serial Monitor
3. UPDATE config.h → ESP32_CAM_IP = "192.168.1.50"
4. Rebuild main ESP32 firmware (PlatformIO)
5. Test stream: http://192.168.1.50/stream
6. Verify telemetry includes "camera_url"
```

## 🔍 Endpoints After Setup
```
http://192.168.1.50/           → Config portal
http://192.168.1.50/stream     → MJPEG stream (<600ms)
http://192.168.1.50/capture    → Single JPEG
```

## 🛠️ Troubleshooting

| **Issue** | **Solution** |
|-----------|--------------|
| **"Camera init failed"** | 5V power, PSRAM enabled, AI Thinker board |
| **"Brownout detector"** | External 5V/1A+ supply (USB insufficient) |
| **"Upload fails"** | GPIO0→GND during power-on, 5V FTDI |
| **"Laggy stream"** | JPEG quality=10, SVGA, WiFi signal |
| **"No WiFi"** | Check antenna, same SSID/password |

## ✅ Advantages of Separate Architecture

```
✅ No GPIO conflicts (motors use 13-33)
✅ Independent failure domains
✅ Main ESP32: 60KB RAM freed (no camera)
✅ Test camera separately
✅ Simpler main firmware
✅ Network-only integration
```

## 🚫 NOT Recommended: UART Bridge
```
ESP32-CAM → Serial JPEG → Main ESP32 → WebSocket
❌ High complexity, RAM pressure, timing issues
✅ Use network architecture above
```

**Next Steps:** Flash ESP32-CAM → Update IP → Test stream + telemetry! 🎥
```
