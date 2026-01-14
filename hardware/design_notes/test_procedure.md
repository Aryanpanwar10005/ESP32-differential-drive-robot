# PCB Acceptance Test Procedure

## Test Equipment Required

-   Adjustable bench power supply (0-10V, current-limited)
-   Digital multimeter (DMM)
-   Oscilloscope (optional, for buck converter ripple check)
-   USB cable
-   Test motors (stall current <1.2A)
-   GPS module
-   Servo motor

## Phase 1: Visual Inspection

1. ✓ Check for solder bridges (use magnification)
2. ✓ Verify component orientation:
    - ICs (pin 1 marking)
    - Electrolytic capacitors (polarity)
    - LEDs (polarity)
    - Diodes (cathode marking)
3. ✓ Inspect solder joints on:
    - USB connector (prone to cold joints)
    - Screw terminals
    - ESP32 module pins

## Phase 2: Power-Up Test (NO LOAD)

**Setup:**

-   Connect 7.4V power supply, **current limit 500mA**
-   **Do NOT connect battery yet**
-   **Do NOT plug in USB yet**

**Procedure:**

1. Apply 7.4V power
2. **Immediately check current:** Should be <50mA
3. **Measure voltages:**
    - Battery input: 7.4V ✓
    - 5V rail: 5.0V ± 0.1V ✓
    - 3.3V rail: 3.3V ± 0.05V ✓
4. **Touch test:** No components should be hot
5. **Check ripple (oscilloscope):** 5V and 3.3V rails should have <50mV ripple
6. Power off

**Pass Criteria:** All voltages within spec, no thermal issues, current <50mA

## Phase 3: Programming Test

**Setup:**

-   Power off
-   Connect USB cable (computer should provide power via USB)

**Procedure:**

1. Plug in USB
2. **Check Device Manager (Windows) or dmesg (Linux):**
    - CH340G should enumerate as COM port
3. Open Arduino IDE / PlatformIO
4. Select ESP32 board, correct COM port
5. Upload blink sketch:
    ```cpp
    void setup() { pinMode(2, OUTPUT); }
    void loop() { digitalWrite(2, !digitalRead(2)); delay(500); }
    ```
6. **Verify:** LED on GPIO2 (if populated) blinks at 1Hz
7. **Open Serial Monitor** (115200 baud)
8. **Verify:** Can send/receive serial data

**Pass Criteria:** Programming succeeds, serial communication works, no errors

## Phase 4: Motor Driver Test (SINGLE MOTOR)

**Setup:**

-   Flash motor test firmware (see `test/motor_test.cpp`)
-   Connect **ONE motor only** to Motor A terminals
-   Power from 7.4V supply, current limit 2A

**Procedure:**

1. Serial command: `FORWARD 128`
2. **Verify:** Motor spins forward at ~50% speed
3. **Measure current:** Should be 200-800mA (no-load)
4. Serial command: `REVERSE 128`
5. **Verify:** Motor reverses direction
6. Serial command: `STOP`
7. **Verify:** Motor stops (braking or coasting depending on firmware)
8. Repeat for Motor B

**Pass Criteria:** Both motors respond correctly, current within expected range, no overheating

## Phase 5: GPS UART Test

**Setup:**

-   Connect GPS module to J5 (4-pin JST)
-   Flash GPS test firmware

**Procedure:**

1. Power on, wait 60 seconds (GPS cold start)
2. Open Serial Monitor
3. **Verify:** NMEA sentences displayed:
    ```text
    $GPGGA,123519,4807.038,N,01131.000,E,...
    $GPRMC,123519,A,4807.038,N,01131.000,E,...
    ```
4. **Check update rate:** Should be 1Hz (1 fix per second)
5. **Verify firmware:** `gps.location.isValid()` returns true

**Pass Criteria:** NMEA sentences received, fix acquired (if outdoors), baud rate correct (9600)

## Phase 6: Camera Test (ESP32-CAM Variant Only)

**Setup:**

-   Flash camera firmware (`pio run -e esp32cam -t upload`)
-   ESP32-CAM must have PSRAM

**Procedure:**

1. Power on
2. Serial Monitor: note IP address
3. Open browser: `http://<ESP32-IP>:81/stream`
4. **Verify:** Live video stream visible
5. **Check frame rate:** Should be ~8 FPS
6. **Check latency:** Move object in front of camera, verify <1 second lag

**Pass Criteria:** Video stream works, acceptable latency, no crashes

## Phase 7: Full Integration Test

**Setup:**

-   Connect all peripherals: motors, GPS, servo
-   Flash production firmware (`pio run -e esp32dev -t upload`)

**Procedure:**

1. Power on
2. **BLE pairing:** Use nRF Connect app, authenticate
3. **Motor control test:**
    - Send MOVE commands via WebSocket
    - Verify differential drive (left/right motor speed control)
4. **GPS telemetry test:**
    - Monitor WebSocket telemetry packets
    - Verify GPS coordinates update every 5 seconds
5. **Servo test:**
    - Send AIM commands
    - Verify servo moves to target angle
6. **Fault handling test:**
    - Disconnect Wi-Fi during motor operation
    - **Verify:** Motors brake immediately (active braking)
    - **Verify:** State transitions to ERROR
7. **Thermal test:**
    - Run full operation for 10 minutes
    - **Touch test:** No components uncomfortably hot (>60°C)

**Pass Criteria:** All subsystems functional, fault handling correct, no thermal issues

## Phase 8: Current Consumption Measurement

**Procedure:**

1. Measure idle current (connected, no motors): \_\_\_ mA
2. Measure with GPS active: \_\_\_ mA
3. Measure with one motor running (no load): \_\_\_ mA
4. Measure with camera streaming (ESP32-CAM): \_\_\_ mA
5. Calculate estimated runtime with 2500mAh battery

**Expected Values:**

-   Idle: 100-150mA
-   GPS active: +30mA
-   Motor (no-load): +200mA per motor
-   Camera: +300mA

## Final Sign-Off

**Checklist:**

-   [ ] All voltage rails within spec
-   [ ] Programming reliable
-   [ ] Motors respond correctly
-   [ ] GPS data received
-   [ ] Camera streams (if applicable)
-   [ ] BLE authentication works
-   [ ] Fault handling tested
-   [ ] No thermal issues
-   [ ] Current consumption acceptable
