# Pin Mapping - Hardware GPIO Assignments

**Note:** GPIOs may differ for custom PCB; this mapping reflects the reference prototype.

## Motor Control (L298N H-Bridge)

| Function        | GPIO Pin | Notes             |
| --------------- | -------- | ----------------- |
| Left Motor PWM  | GPIO 25  | LEDC Channel 0    |
| Left Motor IN1  | GPIO 26  | Direction control |
| Left Motor IN2  | GPIO 27  | Direction control |
| Right Motor PWM | GPIO 32  | LEDC Channel 1    |
| Right Motor IN1 | GPIO 33  | Direction control |
| Right Motor IN2 | GPIO 14  | Direction control |

## Servo Control (Laser Aiming)

| Function  | GPIO Pin | Notes           |
| --------- | -------- | --------------- |
| Servo PWM | GPIO 13  | 50Hz PWM signal |

## GPS Module (UART)

| Function          | GPIO Pin | Notes          |
| ----------------- | -------- | -------------- |
| GPS RX (ESP32 TX) | GPIO 17  | UART2 TX       |
| GPS TX (ESP32 RX) | GPIO 16  | UART2 RX       |
| GPS Baud Rate     | 9600     | NMEA sentences |

## BLE (Built-in)

| Function       | Notes                     |
| -------------- | ------------------------- |
| BLE Radio      | ESP32 internal peripheral |
| Authentication | Custom service UUID       |

## Wi-Fi (Built-in)

| Function    | Notes                 |
| ----------- | --------------------- |
| Wi-Fi Radio | ESP32 internal 2.4GHz |
| Antenna     | PCB trace or external |

## Camera (ESP32-CAM Only)

**For ESP32-CAM (AI-Thinker) builds:**

| Function     | GPIO Pin                    | Notes                |
| ------------ | --------------------------- | -------------------- |
| Camera D0-D7 | GPIO 5,18,19,21,36,39,34,35 | Data lines           |
| Camera XCLK  | GPIO 0                      | Master clock         |
| Camera PCLK  | GPIO 22                     | Pixel clock          |
| Camera VSYNC | GPIO 25                     | Vertical sync        |
| Camera HREF  | GPIO 23                     | Horizontal reference |
| Camera SDA   | GPIO 26                     | I2C data             |
| Camera SCL   | GPIO 27                     | I2C clock            |
| Camera PWDN  | GPIO 32                     | Power down           |
| Camera RESET | -1                          | Not used             |

**Note:** Regular ESP32 builds do not use camera pins.

## Power

| Function    | Voltage | Notes                 |
| ----------- | ------- | --------------------- |
| ESP32 Logic | 3.3V    | Regulated from 5V     |
| Motors      | 5-12V   | Separate power domain |
| Servo       | 5V      | From motor supply     |
| GPS         | 3.3V    | From ESP32 regulator  |

## Safety Notes

-   Motor power domain separated from ESP32 logic
-   Decoupling capacitors on all Vcc pins
-   Flyback diodes on motor outputs (internal to L298N)
