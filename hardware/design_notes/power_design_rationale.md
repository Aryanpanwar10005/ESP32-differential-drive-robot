# Power Design Rationale

## Why Buck Converters, Not Linear Regulators?

### ESP32 3.3V Supply

**Original consideration:** AMS1117-3.3 LDO  
**Problem:**

-   ESP32-CAM can draw 500-700mA peak during WiFi TX + camera capture
-   AMS1117 dropout: 1.1V → requires 4.4V minimum input
-   With 7.4V input: (7.4-3.3) × 0.5A = **2.05W heat dissipation**
-   AMS1117 SOT-223 thermal resistance: ~60°C/W
-   Temperature rise: 2.05W × 60°C/W = **123°C above ambient** → thermal shutdown

**Solution:** MP1584 buck converter (3.3V, 1.5A)

-   Efficiency: ~92%
-   Heat dissipation: 0.5A × 3.3V × 0.08 = **0.13W** (negligible)
-   No thermal issues
-   Supports ESP32-CAM peak current

### Servo 5V Supply

**Original consideration:** 7805 linear regulator  
**Problem:**

-   Servo stall current: ~1.5A (typical SG90)
-   With 7.4V input: (7.4-5) × 1.5A = **3.6W heat dissipation**
-   7805 TO-220 without heat sink: ~50°C/W
-   Temperature rise: 3.6W × 50°C/W = **180°C above ambient** → immediate shutdown

**Solution:** MP1584 buck converter (5V, 2A)

-   Efficiency: ~90%
-   Heat dissipation: 1.5A × 5V × 0.10 = **0.75W** (manageable)
-   No heat sink required

## Interview Defense

**Question:** "Why not use linear regulators for simplicity?"

**Answer:** "Linear regulators dissipate excessive heat with 7.4V input. For ESP32-CAM at 500mA, an AMS1117 would dissipate over 2W, causing thermal shutdown. Buck converters reduce this to <200mW while maintaining efficiency and eliminating the need for heat sinks. This is critical for a battery-powered mobile robot where thermal management and power efficiency directly impact runtime."
