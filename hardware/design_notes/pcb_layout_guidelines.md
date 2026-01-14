# PCB Layout Guidelines

## Board Specifications

-   **Size:** 100mm x 80mm (standard robot chassis compatible)
-   **Layers:** 2-layer (sufficient for this design complexity)
-   **Thickness:** 1.6mm standard FR4
-   **Copper Weight:** 1oz (35µm) for signals, 2oz for motor/power traces if available

## Trace Width Requirements (1oz copper)

-   **Motor Power (7.4V, 2A continuous):** 2.5mm minimum, 3.0mm preferred
-   **5V Rail (2A max):** 1.5mm minimum
-   **3.3V Rail (1A max):** 0.8mm minimum
-   **Signal Traces:** 0.25mm (10mil)
-   **Ground Pour:** Full copper pour on both layers with thermal relief

## Component Placement Strategy

**Power Section (Top-Left):**

1. Battery connector
2. Reverse polarity protection
3. Fuse
4. Bulk capacitors
5. 5V buck regulator
6. 3.3V buck regulator

**Logic Section (Center):**

1. ESP32 module (antenna at board edge)
2. USB connector (top edge, accessible)
3. Boot/Reset buttons
4. Status LEDs (front edge)

**Motor Section (Right/Bottom):**

1. Motor driver IC (TB6612FNG)
2. Motor terminals (board edge)
3. Servo connector
4. Keep >15mm from ESP32

**Peripherals (Left/Bottom):**

1. GPS connector
2. Debug headers

## Critical Design Rules

### Power Integrity

-   Buck regulator components **within 10mm** of IC (inductor, caps)
-   Ground plane **continuous** under inductors
-   Keep switching nodes **short** (hot loop minimization)
-   Ferrite bead placement: between 3.3V source and ESP32

### Motor Driver Isolation

-   Motor traces: **wide, short, direct** to terminals
-   Motor ground return: dedicated path to star point (not through logic ground)
-   100µF decoupling cap within **5mm** of TB6612FNG VM pin

### ESP32 RF Performance

-   Antenna keepout zone: **5mm** radius, no copper on top/bottom layers
-   Antenna edge clearance: **2mm** from board edge
-   No high-current traces near antenna
-   3.3V decoupling caps **within 5mm** of ESP32 VCC pins

### Ground Architecture

-   Top layer: logic ground pour
-   Bottom layer: motor ground pour
-   **Single connection point** at battery input (star ground)
-   0Ω resistor bridge (easy debug/measurement)
-   Verify: motor current returns through dedicated path, NOT logic ground

### Thermal Management

-   TB6612FNG: expose thermal pad, connect to ground pour
-   Buck regulators: thermal vias under IC (4x 0.3mm vias minimum)
-   Avoid placing ICs near board edges (heat sinking)

### ESD Protection

-   USB data lines: place ESD diodes **within 5mm** of connector
-   External connectors: ESD diodes on signal lines
-   Ground plane: continuous path from connectors to star point

## Mounting

-   4x M3 mounting holes at corners (or chassis-specific pattern)
-   Hole diameter: 3.2mm (clearance for M3 screw)
-   Pad diameter: 6mm
-   Keepout radius: 8mm (no components within this zone)

## Manufacturing Notes

-   Minimum trace width: 0.2mm (8mil)
-   Minimum clearance: 0.2mm (8mil)
-   Silkscreen: component designators, polarity marks, voltage labels
-   Soldermask: green (standard), white silkscreen
