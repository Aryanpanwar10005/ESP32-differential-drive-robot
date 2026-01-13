# System Architecture

## Overview

The ESP32 Differential Drive Robot is built on a modular, event-driven architecture using the Arduino framework on the ESP32 platform. The system is designed for high reliability, low latency (<600ms), and strict safety compliance.

---

## Security Model

### BLE Authentication Gate

-   **Requirement:** PDF mandates BLE authentication before control/video access
-   **Implementation:** State machine blocks OPERATIONAL state until BLE auth succeeds
-   **Session Model:** BLE authentication uses session-based trust; credentials are not persisted across reboot
-   **Enforcement Points:**
    -   Motor commands rejected in IDLE state
    -   Camera stream returns 403 before authentication
    -   Telemetry transmission starts only after auth

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
