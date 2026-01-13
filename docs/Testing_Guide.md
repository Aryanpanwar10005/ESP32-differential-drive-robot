# Comprehensive Testing Guide

## ✅ **Hardware Verification Checklist**

```
□ 7.4V LiPo → Fully charged
□ Buck converters → 5V/3A (logic), 6V (motors)
□ **MOTORS LIFTED OFF GROUND** ⚠️
□ All wiring → Double-checked vs Pin_Mapping.md
□ GPS antenna → Outside/clear sky view
□ ESP32-CAM → Separate power + network
```

## 🧪 **Module Tests (Standalone)**

### **1. Motor Test** `test_motor.ino`
```cpp
moveForward(100); delay(2000); stopMotors();
turnLeft(150); delay(2000); stopMotors();
```
**Expected**: Wheels spin correct direction/speed

### **2. GPS Test** `[GPS] Fix OK`
**Expected**: `{"lat":xx,"lon":yy,"fix":true}` after 30-60s outdoors

### **3. Servo Test**
```cpp
setLaserAngle(10); delay(1000);
setLaserAngle(90); delay(1000); 
setLaserAngle(170); delay(1000);
```
**Expected**: Smooth sweep 10°→170°

### **4. BLE Auth Test** (nRF Connect)
```
1. Scan → "ESP32_Robot_001"
2. Connect → Service: 4fafc201-...
3. Char: beb5483e-... → Write "SecureToken123"
4. LED → Solid ON
```

## 🌐 **Integration Tests**

### **Full System Test Sequence**
```
1. Power ON → [INIT] → WAIT_AUTH (blink 500ms)
2. BLE Auth → [CONNECT_NETWORK] → [OPERATIONAL] (solid)
3. Verify telemetry JSON every 2s
4. Send: {"action":"forward","speed":150}
5. Verify: [MAIN] Command: forward (150)
6. **CRITICAL**: Disconnect → EMERGENCY STOP + ERROR blink
7. Auto-recovery after 10s → Reconnect
```

### **Test Server** `python test/websocket_test_server.py`
```
ws://0.0.0.0:8080/ws
Auto-sends forward(100) every 10s
```

## 📈 **Performance Benchmarks**

| Test | Expected | Pass/Fail |
|------|----------|-----------|
| Command Latency | <50ms | |
| GPS Update Rate | 30Hz parse | |
| Telemetry Size | ~250B | |
| Heap Usage | >200KB free | |
| WiFi Reconnect | <5s | |
| BLE Auth Time | <2s | |

## 🛡️ **Safety Verification**

```
□ Emergency stop on disconnect ✓
□ Motors OFF in ERROR state ✓
□ Speed limited 0-200 ✓
□ Auth blocks commands ✓
□ 30s network timeout ✓
□ 10s error recovery ✓
```

**✅ PASS = Ready for field deployment!**
