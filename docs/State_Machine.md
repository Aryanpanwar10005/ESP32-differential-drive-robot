# State Machine Specification

## 📊 **Visual State Diagram**

```
     ┌────────┐
     │  INIT  │ 1s startup
     └────┬───┘
          │ All modules OK
          ▼
 ┌─────────────────┐
 │   WAIT_AUTH     │◄─────┐
 │ LED blink 500ms │      │
 │ BLE scanning    │      │
 └──────┬──────────┘      │
        │ Auth ✓          │
        ▼                 │
 ┌─────────────────┐      │
 │CONNECT_NETWORK  │      │
 │WiFi+WebSocket   │      │
 │30s timeout      │      │
 └──────┬──────────┘      │
        │ Connected ✓     │
        ▼                 │
 ┌─────────────────┐      │
 │  OPERATIONAL    │──────┘
 │LED solid ON     │  Disconnect
 │Telemetry 2Hz    │
 │Commands enabled │
 └──────┬──────────┘
        │ Error
        ▼
 ┌─────────────────┐
 │     ERROR       │
 │Fast blink 250ms │───10s───► CONNECT_NETWORK
 │Motors STOPPED   │
 └─────────────────┘
```

## ⚙️ **State Details**

| State | Entry Actions | Loop Actions | Exit Conditions | LED Behavior |
|-------|---------------|--------------|-----------------|--------------|
| **INIT** | Module init | None | All OK | OFF |
| **WAIT_AUTH** | Start BLE | Auth check | `isAuthenticated()` | **BLINK 500ms** |
| **CONNECT_NETWORK** | WiFi+WS connect | Retry loop | `isConnected()` or 30s | ON (solid during connect) |
| **OPERATIONAL** | Enable commands | Telemetry+commands | Disconnect | **SOLID ON** |
| **ERROR** | `stopMotors()` | Fast blink+countdown | 10s timeout | **BLINK 250ms** |

## ⏱️ **Timing (Non-blocking millis())**

| Event | Interval |
|-------|----------|
| GPS Parse | **Every loop** |
| GPS Print | 5000ms |
| Telemetry | **2000ms** |
| Auth Check | **Every loop** |
| Network Loop | **Every loop** |
| Error Recovery | **10000ms** |

## 🛡️ **Safety Guarantees**

```
❌ ERROR state → NO motor commands
✅ Disconnect → IMMEDIATE stopMotors()
✅ Auth fail → Block ALL commands  
✅ Non-OPERATIONAL → Ignore commands
✅ Speed → constrain(0, MAX_SPEED=200)
```

**Zero runtime motor crashes guaranteed.**
