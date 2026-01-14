# PCB Design - ESP32 Differential Drive Robot

> ⚠️ Phase-2 Status Notice
>
> This directory contains a complete **PCB design framework and documentation**
> aligned with PDF Round-2 requirements.
>
> Final schematic capture and PCB layout are intended to be completed in KiCad
> as the next execution step.

## Overview

Complete hardware design satisfying PDF Round-2 requirements for embedded systems internship.

## Design Specifications

### Power Architecture

-   **Input:** 7.4V LiPo 2S (6-8.4V range)
-   **Motor Supply:** 7.4V direct from battery
-   **ESP32 Logic:** 3.3V via high-efficiency buck regulator (≥1A peak current capability)
    -   **NOT AMS1117** (insufficient for ESP32-CAM current spikes)
    -   Recommended: MP1584, SY8208, or TPS5430 buck converter
    -   Alternative LDO: TLV75733 or MIC5504 (if buck not available)
-   **Servo/Peripheral Supply:** 5V via buck regulator (≥2A)
    -   Recommended: MP1584, LM2596, or TPS54202
    -   **NOT 7805** (thermal and efficiency concerns)
-   **GPS Supply:** 3.3V from ESP32 buck regulator

### Motor Driver

-   **IC:** TB6612FNG (dual H-bridge, preferred) OR L298N (acceptable with heat sink)
-   **Current Rating:** ~1A continuous, 1.2A peak per channel
-   **Motor Selection Requirement:** Stall current must be <1.2A
-   **Protection:** Integrated flyback diodes (TB6612FNG), thermal shutdown
-   **Channels:** 2x for differential drive motors

### ESP32 Module

**Design Variant Strategy:**

-   Base PCB supports **ESP32-WROOM-32**
-   **ESP32-CAM** supported via alternate BOM + DNI (Do Not Install) parts
-   Camera signals routed only for ESP32-CAM variant
-   **One PCB design, two assembly options**

**Module Specs:**

-   **Flash:** 4MB minimum
-   **PSRAM:** Required for ESP32-CAM variant only
-   **Programming:** USB-to-UART (CH340G or CP2102)

### Connectors

-   **Motor Outputs:** 2x screw terminal blocks (left/right drive motors)
-   **Servo Output:** 3-pin header (VCC 5V, GND, Signal)
-   **Battery Input:** XT60 or JST-XH 2-pin with reverse polarity protection
-   **GPS UART:** 4-pin JST-XH (VCC 3.3V, GND, TX, RX)
-   **Programming:** Micro-USB or USB-C
-   **Debug:** 6-pin UART header (optional)

### Safety Features

-   **Reverse Polarity Protection:** P-channel MOSFET or Schottky diode
-   **Decoupling:** 100nF ceramic + 10µF electrolytic per IC
-   **Ferrite Bead:** On 3.3V rail near ESP32 (EMI suppression)
-   **ESD Protection:** Diodes on USB D+/D− and external connectors (recommended)
-   **Current Sensing:** Optional INA219 or shunt resistor for monitoring
-   **Fuse:** 3A automotive blade fuse on battery input
