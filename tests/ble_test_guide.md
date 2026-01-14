# BLE Authentication Testing Guide

## 📱 **nRF Connect Mobile (iOS/Android)**

### **Step-by-Step Authentication**

```
1️⃣  Install: nRF Connect (Nordic Semiconductor)
2️⃣  Power ON ESP32 → Wait [MAIN] → WAIT_AUTH
3️⃣  🔍 SCAN → Find "ESP32_Robot_001"
4️⃣  📲 CONNECT (Pairing optional)
5️⃣  🔎 Services → 4fafc201-1fb5-459e-8fcc-c5c9c331914b
6️⃣  📝 Characteristics → beb5483e-36e1-4688-b7f5-ea07361b26a8
7️⃣  ✏️  WRITE → "SecureToken123" (UTF-8)
8️⃣  ✅ LED solid ON → [BLE] Authentication successful!
```

### **Expected Log Output**
```
[MAIN] Waiting for BLE authentication...
[BLE] Client connected: xx:xx:xx:xx:xx:xx
[BLE] Auth token OK ✓
[BLE] Authentication successful!
[BLE] BLE disabled | Heap freed: +60KB
```

## 🔍 **UUID Reference**

| Purpose | UUID |
|---------|------|
| **Service** | `4fafc201-1fb5-459e-8fcc-c5c9c331914b` |
| **Auth Char** | `beb5483e-36e1-4688-b7f5-ea07361b26a8` |
| **Token** | `SecureToken123` |

### **Common Issues**

| Symptom | Cause | Fix |
|---------|-------|-----|
| **No Device** | BLE OFF | Power cycle ESP32 |
| **Connect Fail** | iPhone popup | Allow "Pairing" |
| **Auth Fail** | Wrong token | Exact: `SecureToken123` |
| **LED Still Blinking** | Wrong UUID | Copy-paste exact UUIDs |

## 📸 **Screenshots Guide**

```
1. Scanner Screen → "ESP32_Robot_001" RSSI -50
2. Connected → 1 Service detected
3. Characteristic → WRITE → "SecureToken123"
4. Serial Monitor → [BLE] Auth OK → LED SOLID
```

**✅ Success = Network connection starts automatically!**
