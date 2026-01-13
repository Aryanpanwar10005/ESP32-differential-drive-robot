# ESP32 Pin Mapping & Wiring Guide

## 🎛️ **Visual Pinout Diagram**

```
         ┌──────────────────────┐
    3V3──┤1                   30│──GND
     EN──┤2                   29│──GPIO23
  SVP36──┤3                   28│──GPIO22 (TX2)
  SVN39──┤4                   27│──GPIO1 (TX0)
   GPIO34┤5                   26│──**GPIO26 LEFT_IN1**
   GPIO35┤6                   25│──**GPIO25 LEFT_PWM**
   **GPIO32 RIGHT_PWM**┤7    24│──GPIO19
   **GPIO33 RIGHT_IN1**┤8    23│──GPIO18
   **GPIO25 LEFT_PWM**┤9     22│──GPIO5
   **GPIO26 LEFT_IN1**┤10    21│──**GPIO17 GPS_TX**
   **GPIO27 LEFT_IN2**┤11    20│──**GPIO16 GPS_RX**
   **GPIO14 RIGHT_IN2**┤12   19│──GPIO4
   GPIO12┤13                 18│──**GPIO2 LED**
    GND──┤14                 17│──**GPIO15 MOTOR_STBY**
   **GPIO13 SERVO**┤15       16│──GPIO0 (BOOT)
         └──────────────────────┘
```

## 🔌 **Connection Tables**

### **TB6612FNG Motor Driver**
| TB6612FNG | ESP32 GPIO | Wire Color |
|-----------|------------|------------|
| PWMA | **GPIO25** | Orange |
| AIN1 | **GPIO26** | Red |
| AIN2 | **GPIO27** | Black |
| PWMB | **GPIO32** | Yellow |
| BIN1 | **GPIO33** | Green |
| BIN2 | **GPIO14** | Blue |
| STBY | **GPIO15** | White |
| VCC | 7.4V Buck | Red |
| GND | GND | Black |

### **NEO-6M GPS Module**
| GPS Pin | ESP32 GPIO | Notes |
|---------|------------|-------|
| VCC | 3.3V/5V |  |
| GND | GND |  |
| TX | **GPIO16** | GPS → ESP32 |
| RX | **GPIO17** | ESP32 → GPS |

### **SG90 Servo (Laser Aiming)**
| Servo Pin | ESP32 GPIO | Power |
|-----------|------------|-------|
| Signal | **GPIO13** |  |
| VCC | 5V Buck | **NOT 3.3V** |
| GND | GND |  |

### **Power Distribution**
```
7.4V LiPo ──┬──► LM2596 (5V/3A) ──► ESP32 3.3V, Servo 5V
            │
            └──► Motor Buck (6V) ──► TB6612FNG VM
```

**⚠️ CRITICAL: Separate logic/motor power!**
