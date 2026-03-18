# GB/T EVSE Controller Implementation Plan

This document defines a step-by-step implementation plan for a production-grade **GB/T 27930** EV charger controller on Linux (i.MX 8M Plus) using SocketCAN, with a clean path toward **OCPP integration** and **power electronics control**.

---

## ✅ Phase 0 — Project Initialization

1. **Initialize repository structure**
   - Create `src/` and required subfolders per architecture:
     - `src/core/`
     - `src/gbt/`
     - `src/can/`
     - `src/power/`
     - `src/ocpp/`
     - `src/config/`
     - `src/utils/`
   - Create a baseline `CMakeLists.txt` with a `gbt_core` library and a `gbt` executable.
   - Add `README.md` and `IMPLEMENTATION_PLAN.md`.

2. **Verify toolchain**
   - Confirm CMake + GCC (or Clang) builds on target platform.
   - Ensure SocketCAN development headers are available (`linux/can.h`, etc.).

---

## ✅ Phase 1 — GB/T CAN Protocol Core (Mandatory Minimum)

### 1.1 GB/T Frame Layer
- Implement `gbt::GbtFrame` with strict encoding/decoding per GB/T.
- Include frame types: CHM, BHM, BRM, CRM, BCP, BRO, CRO, BCL, CCS, BCS, BST, CST.
- Validate checksum and field values.

### 1.2 CAN Transport Layer
- Define `can::CanInterface` abstraction.
- Implement `can::SocketCan` using SocketCAN (blocking/non-blocking).
- Add filter support and DLC validation.

### 1.3 GB/T State Machine
- Implement `gbt::StateMachine` (IDLE → HANDSHAKE → PARAMETER → READY → CHARGING → STOP → FAULT).
- Input: received GB/T frames.
- Output: frames to send + desired V/I + state.

### 1.4 System Orchestration
- Create `core::System` (or `core::Controller`) to:
  - Drive CAN RX (receive frames, feed state machine).
  - Periodically run control loop (100 ms) to send frames/commands.
  - Expose a simple API for `StartCharging()`, `StopCharging()`, etc.

### 1.5 Verification
- Unit tests for frame encoding/decoding.
- Simulation test: feed canned CAN frames through the stack and verify state transitions.

---

## ✅ Phase 2 — Power Electronics Interface (Your Implementation)

### 2.1 Power Module Interface
- Define `power::PowerModuleInterface` with at least:
  - `StartModule(float voltage, float current, VehicleState state)`
  - `StopModule()`
  - `ReadFeedback()`

### 2.2 Integrate into GB/T Control Loop
- When GB/T state machine indicates `CHARGING` and provides target V/I:
  - Call `StartModule(targetV, targetI, state)`.
- When GB/T indicates `STOP` or `FAULT`:
  - Call `StopModule()`.

### 2.3 Feedback Loop (optional for Phase 2)
- Allow `PowerModule` to feed back actual V/I to the GB/T stack for CCS frame reporting.

---

## ✅ Phase 3 — OCPP Integration (Extensible)

### 3.1 OCPP Client Core
- Implement OCPP 1.6J client (WebSocket + JSON-RPC).
- Provide config for CSMS URL / authentication.

### 3.2 Core Events → OCPP
- Map charger states to OCPP states.
- Send:
  - `StatusNotification`
  - `MeterValues`
  - `StartTransaction` / `StopTransaction`

### 3.3 OCPP Commands → Charger Control
- Handle `RemoteStartTransaction` / `RemoteStopTransaction` callbacks.
- Provide a callback into `core::System`.

---

## ✅ Phase 4 — Production Hardening

1. **Configuration**
   - Add `config::Config` for JSON/YAML settings (CAN interface, OCPP URL, power limits).

2. **Logging & Diagnostics**
   - Add leveled logging (INFO/WARN/ERROR) with timestamps.
   - Add optional persistent diagnostics (circular buffer).

3. **Watchdog & Health Monitoring**
   - Implement a watchdog thread that stops charge on timeouts/faults.

4. **Safety & Fault Handling**
   - Define fault categories and transitions.
   - Implement safe shutdown on fault.

---

## 🧩 File Structure (Target)

```
src/
 ├── main.cpp
 ├── core/
 │    ├── StateMachine.hpp/cpp
 │    ├── ChargerContext.hpp
 │    ├── Scheduler.hpp
 │    └── Watchdog.hpp/cpp
 ├── gbt/
 │    ├── GbtController.hpp/cpp
 │    ├── GbtParser.hpp/cpp
 │    ├── GbtFrames.hpp
 │    └── GbtStateMachine.cpp
 ├── can/
 │    ├── CanInterface.hpp
 │    └── SocketCan.cpp
 ├── power/
 │    ├── PowerModule.hpp
 │    ├── PowerManager.hpp
 │    └── UUGreenDriver.cpp
 ├── ocpp/
 │    ├── OcppClient.hpp
 │    └── OcppClient.cpp
 ├── config/
 │    └── Config.hpp/cpp
 └── utils/
      ├── Logger.hpp/cpp
      └── Timer.hpp
```

---

## 🎯 Next action (choose 1)
Pick one to start with:

1. **Build Phase 1 (GB/T + SocketCAN)** — I’ll scaffold the full protocol stack and a runnable demo.
2. **Build Phase 2 (Power drive stub + API)** — I’ll add the `StartModule(...)` hook and integrate it.
3. **Build Phase 3 (OCPP client skeleton)** — I’ll implement a minimal OCPP client with status reporting.

Reply with the number you want to start with and I’ll generate the first code scaffold. (Or say “All” and I’ll create the full baseline structure with TODO stubs.)
