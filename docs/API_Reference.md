# API Reference - Command & Telemetry Specification

## Overview

This document defines the command and telemetry format for the ESP32 differential drive robot, satisfying PDF Round-1 deliverable requirements.

---

## Command API (WebSocket → Robot)

Commands are sent as JSON objects over the WebSocket connection.

### **MOVE Command**

Controls differential drive motors.

**Format:**

```json
{
  "command": "MOVE",
  "left_speed": -255 to 255,
  "right_speed": -255 to 255
}
```

**Parameters:**

-   `left_speed`: PWM value for left motor (-255 = full reverse, 0 = stop, 255 = full forward)
-   `right_speed`: PWM value for right motor

**Example:**

```json
{ "command": "MOVE", "left_speed": 200, "right_speed": 200 }
```

**Response:** None (fire-and-forget)

### **STOP Command**

Stops both motors.

**Format:**

```json
{
    "command": "STOP"
}
```

**Behavior:**

-   Normal operation: Motors coast to stop
-   Safety conditions (ERROR state, network loss): Active braking applied automatically by firmware

**Response:** None

### **AIM Command**

Controls laser aiming servo position.

**Format:**

```json
{
  "command": "AIM",
  "angle": 0 to 180
}
```

**Parameters:**

-   `angle`: Servo angle in degrees (0 = minimum, 90 = center, 180 = maximum)

**Example:**

```json
{ "command": "AIM", "angle": 90 }
```

**Response:** None

---

## Telemetry API (Robot → Server)

Telemetry is sent as JSON objects over WebSocket at regular intervals.

### Telemetry Packet Format

```json
{
    "bot_id": "ESP32_A1B2C3",
    "timestamp": 123456789,
    "gps": {
        "latitude": 28.6139,
        "longitude": 77.209,
        "fix_valid": true
    },
    "network": {
        "connected": true,
        "rssi": -62,
        "reconnect_count": 0
    },
    "state": "OPERATIONAL",
    "ble_authenticated": true
}
```

### Field Definitions

| Field                   | Type    | Description                                            |
| ----------------------- | ------- | ------------------------------------------------------ |
| bot_id                  | string  | Unique robot identifier (MAC-based)                    |
| timestamp               | number  | Milliseconds since boot                                |
| gps.latitude            | number  | GPS latitude in decimal degrees                        |
| gps.longitude           | number  | GPS longitude in decimal degrees                       |
| gps.fix_valid           | boolean | true if GPS fix is valid and fresh (≤5s old)           |
| network.connected       | boolean | WebSocket connection status                            |
| network.rssi            | number  | Wi-Fi signal strength (dBm)                            |
| network.reconnect_count | number  | Number of reconnection attempts                        |
| state                   | string  | Current state machine state (IDLE, OPERATIONAL, ERROR) |
| ble_authenticated       | boolean | BLE authentication status                              |

**Note:** Additional GPS fields (altitude, satellites) may be included when available from the GPS module.

---

## GPS Update Requirement

**PDF Requirement:** Position packet must be received and forwarded at ≤ 5-second intervals.

**Implementation:** GPS fix expires after 5 seconds without update. `gps.fix_valid` reflects current validity.

---

## Security & Access Control

### BLE Authentication Required:

-   Commands are rejected until BLE authentication succeeds
-   Camera stream access returns 403 before authentication
-   Telemetry is sent only after authentication

### Network Security:

-   WebSocket communication over Wi-Fi
-   Camera streaming over HTTP port 81
-   Bot operates on trusted arena network

---

## Error Handling

### Network Disconnect:

-   Motors immediately apply active braking
-   State transitions to ERROR
-   Reconnection attempts automatically

### GPS Loss:

-   `gps.fix_valid` becomes false after 5s timeout
-   Telemetry continues with last known position + validity flag

### BLE Disconnect:

-   Idempotent disconnect handling
-   Authentication state reset on reconnection
