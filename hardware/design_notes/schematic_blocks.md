# Schematic Block Requirements

## Block 1: Power Input & Protection

-   Battery connector (XT60)
-   Reverse polarity protection (P-channel MOSFET or Schottky diode)
-   3A fuse (automotive blade type)
-   Power LED indicator (green, with 1kΩ resistor)
-   Bulk capacitor (1000µF 16V electrolytic, low ESR)

## Block 2: Voltage Regulation (CRITICAL - BUCK CONVERTERS REQUIRED)

**5V Buck Regulator (≥2A capacity):**

-   IC: MP1584, LM2596, or TPS54202
-   Input: 7.4V battery
-   Output: 5.0V ±2%
-   Decoupling: 10µF ceramic input + 100µF ceramic output + inductor
-   For: Servo motor, USB VBUS, peripherals

**3.3V Buck Regulator (≥1A capacity):**

-   IC: MP1584, SY8208, or TPS5430
-   Input: 7.4V battery OR 5V rail
-   Output: 3.3V ±2%
-   Decoupling: 10µF ceramic input + 100µF ceramic output
-   Ferrite bead on output before ESP32 (EMI suppression)
-   For: ESP32, GPS module

**⚠️ Design Note:** Linear regulators (AMS1117, 7805) are **NOT recommended** due to thermal dissipation and dropout voltage concerns with ESP32-CAM current spikes.

## Block 3: ESP32 Module

-   ESP32-WROOM-32 footprint (base design)
-   Boot button (GPIO0 to GND, 10kΩ pull-up)
-   Reset button (EN to GND, 10kΩ pull-up, 0.1µF debounce cap)
-   Strapping resistors:
    -   GPIO0: 10kΩ pull-up (pulled LOW for programming mode)
    -   GPIO2: 10kΩ pull-down
    -   GPIO15: 10kΩ pull-up
-   Decoupling capacitors on ALL VCC pins (100nF ceramic + 10µF tantalum)
-   Antenna keepout zone (5mm clearance)
-   Programming mode: GPIO0 pulled LOW during upload

## Block 4: Motor Driver (TB6612FNG Preferred)

-   VM power from battery (7.4V)
-   VCC logic from 3.3V buck regulator
-   Motor A (Left):
    -   PWM: GPIO 25
    -   IN1: GPIO 26
    -   IN2: GPIO 27
-   Motor B (Right):
    -   PWM: GPIO 32
    -   IN1: GPIO 33
    -   IN2: GPIO 14
-   Standby pin: tied HIGH or GPIO control
-   Motor output: 2x screw terminals (2-pin, 5.08mm pitch)
-   Decoupling: 100µF electrolytic on VM, 100nF ceramic on VCC

## Block 5: Servo (Laser Motor)

-   3-pin header (5V, GND, PWM signal)
-   PWM: GPIO 13
-   Power: 5V buck regulator output
-   Decoupling: 100µF electrolytic near connector

## Block 6: GPS Module

-   4-pin JST connector (VCC 3.3V, GND, TX, RX)
-   UART connection:
    -   GPS TX → ESP32 GPIO16 (RX2)
    -   GPS RX → ESP32 GPIO17 (TX2)
-   No level shifting needed (3.3V GPS modules)
-   ESD protection diodes on TX/RX lines (recommended)

## Block 7: Programming & Debug

-   USB-to-UART bridge (CH340G or CP2102)
-   Micro-USB connector (or USB-C)
-   Auto-reset circuit:
    -   DTR → GPIO0 via 100nF capacitor
    -   RTS → EN via 100nF capacitor
-   Status LEDs:
    -   TX (red, active LOW)
    -   RX (red, active LOW)
    -   Power (green)
-   6-pin debug header (VCC, GND, TX, RX, DTR, RTS) - optional

## Block 8: Status Indicators

-   Power LED (green, 1kΩ resistor)
-   BLE Status LED (blue, GPIO 4, 1kΩ resistor)
-   Error LED (red, GPIO 5, 1kΩ resistor)
-   User LED (optional, GPIO 2)

---

## ESP32 Variant Support

Two hardware variants are supported by this design framework:

-   **ESP32-WROOM-32**

    -   External camera optional
    -   Used for non-camera firmware builds

-   **ESP32-CAM (AI-Thinker)**
    -   Integrated OV2640 camera
    -   Requires PSRAM
    -   Used to satisfy Round-1 camera streaming requirement

---

## Block 9: Ground Architecture (CRITICAL)

**Ground Separation Strategy:**

-   Motor ground plane (high current, noisy)
-   Logic ground plane (low current, clean)
-   **Single-point star connection** at power input near battery connector
-   Motor driver VM return path: dedicated trace to star point
-   **NO motor current allowed to flow through ESP32 ground plane**
-   Bridge motor/logic grounds with 0Ω resistor at star point for easy debugging
