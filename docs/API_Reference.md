# WebSocket API Reference v1.0.0

## 📡 **Message Formats**

### → **Telemetry (ESP32 → Server)** `type: "telemetry"`

```json
{
  "type": "telemetry",
  "bot_id": "BOT_001",
  "authenticated": true,
  "state": "OPERATIONAL",
  "gps": {
    "lat": 28.613939,
    "lon": 77.209021,
    "fix": true,
    "satellites": 8,
    "altitude": 216.0
  },
  "camera_url": "http://192.168.1.50/stream",
  "uptime": 123456,
  "free_heap": 234567
}
```

**Sent every 2 seconds in OPERATIONAL state**

### ← **Control Commands (Server → ESP32)** `type: "control"`

| Action | Speed Range | Angle Range | Description |
|--------|-------------|-------------|-------------|
| `forward` | 0-200 | - | Both motors forward |
| `backward` | 0-200 | - | Both motors reverse |
| `left` | 0-200 | - | Differential left turn |
| `right` | 0-200 | - | Differential right turn |
| `stop` | - | - | Emergency stop |
| `servo` | - | **10-170°** | Laser aiming servo |

```json
// Motor Commands
{"type": "control", "action": "forward", "speed": 150}
{"type": "control", "action": "stop"}

// Servo Command  
{"type": "control", "action": "servo", "angle": 90}
```

### ❌ **Error Responses**

```json
{
  "type": "error", 
  "message": "Unauthorized command blocked",
  "code": "AUTH_FAIL"
}
{
  "type": "error",
  "message": "Invalid JSON format",
  "code": "PARSE_ERROR"
}
```

## 🔒 **Security**

- **BLE Authentication** required before commands
- **State gating**: Commands ignored in non-OPERATIONAL
- **Speed limits**: Hard-constrained 0-200 PWM
- **Emergency stop** on disconnect

## 📊 **Performance**

| Metric | Value |
|--------|-------|
| **Telemetry Rate** | 2Hz (500ms) |
| **Command Latency** | <50ms |
| **GPS Update** | Every loop (30Hz) |
| **Max Speed** | 200/255 PWM |

**JSON Size**: ~250 bytes/telemetry
