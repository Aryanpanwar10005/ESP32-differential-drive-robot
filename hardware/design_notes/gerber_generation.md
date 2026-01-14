# Gerber File Generation (KiCad)

## Procedure

1. Open PCB file in KiCad Pcbnew
2. **File → Plot**
3. **Select layers:**
    - ✓ F.Cu (front copper)
    - ✓ B.Cu (back copper)
    - ✓ F.Mask (front soldermask)
    - ✓ B.Mask (back soldermask)
    - ✓ F.Silkscreen (front silkscreen)
    - ✓ B.Silkscreen (back silkscreen)
    - ✓ Edge.Cuts (board outline)
4. **Format:** Gerber (RS-274X)
5. **Output directory:** `hardware/gerbers/`
6. **Click "Plot"**
7. **Generate Drill Files:**
    - Click "Generate Drill Files"
    - Format: Excellon
    - Units: Millimeters
    - Click "Generate Drill File"
8. **Verify output:**
    - Check that all `.gbr` and `.drl` files are generated
    - Use Gerber viewer (KiCad GerbView or online viewer) to inspect
9. **Create ZIP:**
    - Compress all files in `hardware/gerbers/` folder
    - Name: `esp32_robot_gerbers_vX.X.zip`

## Manufacturer Settings (JLCPCB / PCBWay)

-   **PCB thickness:** 1.6mm
-   **Copper weight:** 1oz (2oz for power traces if available)
-   **Layers:** 2
-   **Soldermask color:** Green (standard, cheapest)
-   **Silkscreen color:** White
-   **Surface finish:** HASL (lead-free) or ENIG (better for fine pitch)
-   **Min track/spacing:** 6/6 mil (0.15mm)
-   **Min drill size:** 0.3mm
-   **Castellated holes:** No
-   **Gold fingers:** No

## Upload Checklist

Before uploading to manufacturer:

-   ✓ Gerber files reviewed in viewer
-   ✓ Board outline correct (100x80mm)
-   ✓ No unconnected copper pours
-   ✓ All mounting holes present
-   ✓ Silkscreen readable and not over pads
-   ✓ Drill file included
-   ✓ Quantity and specs match requirements
