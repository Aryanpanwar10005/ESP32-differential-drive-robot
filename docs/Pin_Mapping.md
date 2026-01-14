# Pin Mapping - Hardware GPIO Assignments

**Note:** This mapping reflects the Phase 2 Reference Schematic for the ESP32 Differential Drive Robot.

## Motor Control (TB6612FNG)

| Function        | GPIO Pin | Hardware Connection | Notes                      |
| --------------- | -------- | ------------------- | -------------------------- |
| Left Motor PWM  | GPIO 25  | TB6612FNG PWMA      | LEDC Channel 0             |
| Left Motor IN1  | GPIO 26  | TB6612FNG AIN1      | Direction control          |
| Left Motor IN2  | GPIO 27  | TB6612FNG AIN2      | Direction control          |
| Right Motor PWM | GPIO 32  | TB6612FNG PWMB      | LEDC Channel 1             |
| Right Motor IN1 | GPIO 33  | TB6612FNG BIN1      | Direction control          |
| Right Motor IN2 | GPIO 14  | TB6612FNG BIN2      | Direction control          |
| Motor STBY      | GPIO 15  | TB6612FNG STBY      | Must be HIGH for operation |

## Servo Control (Laser Aiming)

| Function  | GPIO Pin | Notes           |
| --------- | -------- | --------------- |
| Servo PWM | GPIO 13  | 50Hz PWM signal |

## GPS Module (UART)

| Function          | GPIO Pin | Hardware Connection | Notes          |
| ----------------- | -------- | ------------------- | -------------- |
| GPS RX (ESP32 RX) | GPIO 16  | GPS TX              | UART2 RX       |
| GPS TX (ESP32 TX) | GPIO 17  | GPS RX              | UART2 TX       |
| GPS Baud Rate     | 9600     | -                   | NMEA sentences |

## Status Indicator

| Function   | GPIO Pin | Notes                      |
| ---------- | -------- | -------------------------- |
| Status LED | GPIO 2   | Onboard LED / External LED |

## Power Architecture

| Function       | Voltage | Source                  | Notes                 |
| -------------- | ------- | ----------------------- | --------------------- |
| Main Battery   | 7.4V    | 2S LiPo                 | High current path     |
| Logic Rail (1) | 5V      | Buck Converter (LM2596) | For Servo & ESP32 Reg |
| Logic Rail (2) | 3.3V    | Buck/LDO (AMS1117)      | For ESP32 & Sensors   |
| Motor Supply   | 7.4V    | Direct from Battery     | Via TB6612FNG VM      |

## Safety & Critical Design Notes

-   **Reverse Polarity Protection:** P-channel MOSFET on BATT+ input.
-   **Bulk Decoupling:** 1000µF capacitor on the main 7.4V rail.
-   **Servo Isolation:** Servo is powered from the 5V rail to avoid brownouts on the 3.3V logic rail.
-   **Motor Isolation:** Motor power (VM) is separate from logic power (VCC).
-   **STBY Pin:** GPIO 15 must be driven HIGH in software to enable the motor driver.
