# System Architecture

## Overview

The ESP32 Differential Drive Robot is built on a modular, event-driven architecture using the Arduino framework on the ESP32 platform. The system is designed for high reliability, low latency (<600ms), and strict safety compliance.

---

## Architecture Overview (Dual-Processor Model)

The system utilizes a dual-processor architecture to maintain high performance and isolation:

1. **Main ESP32 (WROOM/Dev):** Handles high-level logic, BLE authentication, motor/servo control, GPS parsing, and WebSocket telemetry communication.
2. **ESP32-CAM (Separate Module):** Dedicated to high-speed MJPEG image capture and streaming. This module runs independent firmware configured for the specific camera pins (AI-Thinker) and communicates its status/URL to the main processor.

---

## Security Model

### BLE Authentication Gate

-   **Requirement:** PDF mandates BLE authentication before control/video access.
-   **Multi-Layer Enforcement:**
    -   **State Gate:** The main state machine blocks transitions to `OPERATIONAL` until `bleAuth.isAuthenticated()` is true.
    -   **Function Gate:** Crucial motor functions (`setMotorSpeeds`) perform a redundant check on the BLE auth flag at the code level ("Belt and Suspenders" approach).
    -   **Session Model:** Session-based trust; RAM optimization disables the BLE stack after successful authentication (freeing ~60KB RAM).

### Network Security

-   **Transport:** WebSocket over Wi-Fi for commands and telemetry
-   **Camera Stream:** HTTP on port 81 (BLE-gated at application layer)
-   **TLS Status:** Operating on trusted arena network (isolated from public internet)
-   **Rationale:** Arena network is physically isolated; TLS overhead would impact <600ms latency requirement

### Bot Identification

-   **Unique ID:** Generated from MAC address (last 3 bytes)
-   **Format:** `ESP32_XXXXXX` where X = hex MAC digits
-   **Purpose:** Supports multiple robots on same network (PDF scalability requirement)
-   **Transmission:** Included in every telemetry packet

### Fault Recovery

-   **Network Loss:** Motors brake immediately, reconnection automatic
-   **GPS Timeout:** Fix validity flag prevents stale coordinate transmission
-   **BLE Disconnect:** Idempotent handling, clean reconnection

### Attack Surface

-   **Physical:** Device operates in controlled arena environment
-   **Network:** Trusted Wi-Fi, no public exposure
-   **Authentication:** BLE pairing required before any robot control
