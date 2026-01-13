# ESP32-CAM Streaming Firmware

## Hardware
- AI-Thinker ESP32-CAM module
- OV2640 camera (included)
- FTDI programmer (for upload)
- 5V/1A power supply

## Arduino IDE Setup

1. **Add ESP32 Board:**
   - File → Preferences → Additional Boards URLs:
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Tools → Board → Boards Manager → "ESP32" → Install

2. **Select Board:**
   - Tools → Board → AI Thinker ESP32-CAM
   - Tools → Partition Scheme → Huge APP (3MB)
   - Tools → Upload Speed → 115200

3. **Configure WiFi:**
   Edit in `esp32_cam_streaming.ino`:
   ```cpp
   const char* ssid = "YourWiFiSSID";     // Match main ESP32
   const char* password = "YourPassword";  // Match main ESP32
   IPAddress local_IP(192, 168, 1, 50);   // Match config.h
   ```

## Upload Instructions

### Hardware Connections

| FTDI | ESP32-CAM |
|------|-----------|
| GND  | GND       |
| 5V   | 5V        |
| TX   | U0R       |
| RX   | U0T       |

### Upload Steps

1. Connect FTDI adapter
2. Connect IO0 → GND (upload mode)
3. Power on ESP32-CAM
4. Arduino IDE → Upload
5. Wait for "Connecting..."
6. Disconnect IO0 from GND
7. Press RESET button

## Testing

### Serial Monitor (115200 baud)

Expected output:
```
=== ESP32-CAM Streaming v1.0 ===
[CAMERA] Initialized
[WIFI] IP: 192.168.1.50
[STREAM] http://192.168.1.50/stream
```

### Access Stream

- **Browser:** http://192.168.1.50/stream
- **VLC:** Open Network Stream → http://192.168.1.50/stream
- **Main ESP32:** Automatically detects via camera manager

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Brown-out reset | Use 5V/1A external supply (not USB) |
| Camera init failed | Check ribbon cable connection |
| WiFi timeout | Verify credentials match main ESP32 |
| No upload | Ensure IO0→GND during upload only |

## Integration

Main ESP32 firmware includes `camera_manager` module that:

- Checks ESP32-CAM health every 10s
- Reports camera status in telemetry
- Provides stream URL to server

No manual configuration needed after upload.
