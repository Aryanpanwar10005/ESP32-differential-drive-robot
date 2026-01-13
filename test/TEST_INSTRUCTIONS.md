# 🧪 ESP32 Robot Hardware Testing Instructions

## Overview
Standalone test utilities for **individual module verification** before full system integration. Use these to isolate hardware issues.

## Prerequisites
- **ESP32-WROOM-32** connected via USB
- **PlatformIO** (`pio` command available)
- Hardware wired per `docs/Pin_Mapping.md`
- **Lift motors off ground** for motor tests!

---

## 1. Motor Test 🚗

### Purpose
Verify **TB6612FNG motor driver** + DC motor connections (no BLE/GPS/network).

### Hardware Required
```
✅ TB6612FNG driver
✅ 2x DC motors (6V)
✅ 7.4V battery / bench supply  
✅ ⚠️ WHEELS OFF GROUND or motors lifted
```

### Run Test
```bash
pio run -e motor_test -t upload -t monitor
```

### Expected Output
```
🚀 === Motor Test v1.0 - Standalone Mode ===
⚠️  CRITICAL: LIFT MOTORS OFF GROUND!
[MOTOR] L:+100 R:+100 | Cycle 1/3
[TEST] Forward 100 - 5s
...
✅ ALL TESTS COMPLETE - MOTORS STOPPED
```

### Troubleshooting
| Issue | Check |
|-------|-------|
| No movement | STBY pin (GPIO15), 7.4V power |
| One motor dead | Wiring, swap L/R channels |
| Motors hot | Reduce speed, check stall current |
| Jerky motion | PWM freq (5kHz), driver fault |

---

## 2. GPS Test 🛰️

### Purpose
Verify **NEO-6M GPS** UART + NMEA parsing.

### Hardware Required
```
✅ NEO-6M GPS module
✅ Outdoor location / window view
✅ Antenna unobstructed
```

### Run Test
```bash
pio run -e gps_test -t upload -t monitor
```

### Expected Output
```
🛰️ === GPS Test v1.0 - Standalone Mode ===
[GPS] Sentences: 45 | Fix: ✅ | Satellites: 8
📍 Lat: 28.613900 | Lon: 77.209000
Fix: YES | Satellites: 8 | HDOP: 1.2
```

### Troubleshooting
| Issue | Check |
|-------|-------|
| No data | TX/RX wiring (GPS TX→GPIO16), 3.3V power |
| No fix | **Move outdoors**, 30-60s cold start |
| Wrong baud | Verify GPS default 9600 (some 38400) |

---

## 🔄 Test Sequence Recommendation

```bash
# 1. Motors first (safest)
pio run -e motor_test -t upload -t monitor

# 2. GPS (needs outdoors)
pio run -e gps_test -t upload -t monitor  

# 3. Main firmware
pio run -e esp32dev -t upload -t monitor

# 4. BLE (phone app)
# 5. Network (websocket_test_server.py)
```

## Pro Tips
- **Always** check `pio run -t clean` between tests
- Use `Ctrl+C` to stop tests safely
- Monitor `ESP.getFreeHeap()` for memory leaks
- Test sequence prevents **hardware damage**

**✅ PASS criteria**: All tests complete without errors + expected serial output.

---
*Built for internship submission - professional grade testing suite*
