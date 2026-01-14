# Motor Driver Selection Rationale

## Requirements

-   2 DC motors for differential drive
-   Estimated continuous current: 0.8A per motor
-   Estimated stall current: 1.2-1.5A per motor (depends on motor selection)
-   Operating voltage: 6-8.4V (2S LiPo)

## Candidate Comparison

| Driver    | Channels | Current/Ch          | Efficiency | Thermal Protection | Cost | Decision                  |
| --------- | -------- | ------------------- | ---------- | ------------------ | ---- | ------------------------- |
| L298N     | 2        | 2A                  | ~70%       | No                 | $2   | Acceptable with heat sink |
| TB6612FNG | 2        | ~1A cont, 1.2A peak | ~95%       | Yes                | $3   | **Preferred**             |
| DRV8833   | 2        | 1.5A                | ~95%       | Yes                | $2.5 | Good alternative          |

## Selection: TB6612FNG

**Advantages:**

-   Higher efficiency (95% vs 70%) reduces battery drain
-   Lower voltage dropout (better for LiPo near discharge)
-   Integrated flyback diodes (external diodes not needed)
-   Thermal shutdown protection prevents damage
-   Smaller footprint (SSOP24 vs TO-220)

**Limitations:**

-   Current rating: ~1A continuous, 1.2A peak per channel
-   **Motor selection constraint:** Must use motors with stall current <1.2A

**Trade-off:** Slightly higher cost ($1 more than L298N) is justified by efficiency, thermal performance, and compact size.

## Interview Defense

**Question:** "What if motors exceed 1.2A?"

**Answer:** "Motor selection is constrained to match driver capability. For higher current requirements, I would use DRV8871 (3.6A) or implement current sensing with INA219 to trigger software protection before hardware thermal shutdown. The PDF requirements don't specify motor current, so TB6612FNG is appropriate for typical small robot applications."
