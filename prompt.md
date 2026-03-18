# 📄 Prompt: Production-Grade GB/T EV Charger Controller (C++)

## 🎯 Objective

Design and implement a **modular, production-grade EVSE controller** supporting **GB/T 27930 DC charging** using **C++ (C++17 or later)** on **Linux (i.MX 8M Plus)** with **SocketCAN**, controlling **multiple 30 kW DC power modules**, and extensible for **OCPP-based CSMS communication**.

---

# 🧠 1. System Requirements

### Platform

* CPU: i.MX 8M Plus
* OS: Linux (Yocto/Debian)
* Interfaces:

  * CAN (GB/T 27930, 250 kbps)
  * Power modules (CAN / RS485 / UART)
  * Ethernet/LTE (OCPP)

---

# ⚙️ 2. Functional Scope

### A. GB/T Protocol Controller

* Implement full **GB/T 27930 state machine**:

```
IDLE → HANDSHAKE → PARAMETER → READY → CHARGING → STOP → FAULT
```

* Handle all mandatory frames:

  * Handshake: CHM, BHM
  * Identification: BRM, CRM
  * Parameter: BCP
  * Ready: BRO, CRO
  * Charging loop: BCL, CCS, BCS
  * Termination: BST, CST

---

### B. Power Control

* Control N power modules (scalable)
* Current sharing
* Voltage regulation
* Ramp control (slew rate)
* Fault detection and shutdown

---

### C. OCPP Integration (Extensible)

* OCPP 1.6J minimum
* Map charger states → OCPP statuses
* Send:

  * StatusNotification
  * MeterValues
  * Start/StopTransaction

---

# 🧱 3. Software Architecture (MANDATORY)

```cpp
src/
 ├── main.cpp

 ├── core/
 │    ├── StateMachine.hpp
 │    ├── ChargerContext.hpp
 │    ├── Scheduler.hpp
 │    └── Watchdog.hpp

 ├── gbt/
 │    ├── GbtController.hpp
 │    ├── GbtController.cpp
 │    ├── GbtParser.hpp
 │    ├── GbtParser.cpp
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
 │    └── Config.hpp

 └── utils/
      ├── Logger.hpp
      └── Timer.hpp
```

---

# 🔁 4. Code Flow (Execution Model)

```cpp
MAIN THREAD
 ├── Init CAN
 ├── Init Power Modules
 ├── Init OCPP
 ├── Start Threads:
 │     ├── CAN RX Thread (high priority)
 │     ├── Control Loop (100 ms)
 │     ├── OCPP Client Thread
 │     └── Watchdog Thread
```

---

## 🔄 Runtime Flow

```cpp
[CAN RX]
   ↓
Parse Frame
   ↓
Update Context
   ↓
State Machine Transition
   ↓
Set Target V/I (from BCL)

[CONTROL LOOP - 100 ms]
   ↓
Apply V/I to Power Modules
   ↓
Read Feedback
   ↓
Send CCS

[WATCHDOG]
   ↓
If BCL timeout → STOP
If fault → FAULT state

[OCPP]
   ↓
Send status + metering
```

---

# 📡 5. GB/T Frame Definitions (STRICT)

## 🔹 BCL (Control Command - EV → Charger)

```cpp
ID: 0x1810F456
DLC: 8

Byte 0-1: Target Voltage (uint16, 0.1V)
Byte 2-3: Target Current (uint16, 0.1A)
Byte 4-5: Charging Mode
Byte 6-7: Reserved
```

---

## 🔹 CCS (Charger Status - Charger → EV)

```cpp
ID: 0x1811F456
DLC: 8

Byte 0-1: Output Voltage (0.1V)
Byte 2-3: Output Current (0.1A)
Byte 4: Charger Status
Byte 5: Error Code
Byte 6-7: Reserved
```

---

## 🔹 BCP (Charging Parameters)

```cpp
ID: 0x1806F456

Byte 0-1: Max Voltage (0.1V)
Byte 2-3: Max Current (0.1A)
Byte 4-5: Energy (0.1kWh)
Byte 6-7: SOC
```

---

## 🔹 Scaling Rules

```cpp
Voltage = raw * 0.1
Current = raw * 0.1
Energy  = raw * 0.1
```

---

# 🧠 6. Core Data Structures

```cpp
enum class ChargerState {
    IDLE,
    HANDSHAKE,
    PARAMETER,
    READY,
    CHARGING,
    STOP,
    FAULT
};

struct ChargerContext {
    ChargerState state;

    float targetVoltage;
    float targetCurrent;

    float actualVoltage;
    float actualCurrent;

    uint8_t soc;

    bool fault;
};
```

---

# 🔌 7. Control Logic (MANDATORY)

```cpp
// From BCL
targetVoltage = parsed_voltage;
targetCurrent = parsed_current;

// Apply
powerManager.apply(targetVoltage, targetCurrent);

// Feedback
actualVoltage = power.readVoltage();
actualCurrent = power.readCurrent();

// Send CCS
sendCCS(actualVoltage, actualCurrent);
```

---

# ⚠️ 8. Safety & Timing Constraints

### Timing

* BCL timeout: **≤ 500 ms**
* CCS transmit: **≤ 100 ms cycle**

### Safety Shutdown Conditions

* CAN communication loss
* Overvoltage / overcurrent
* Power module fault
* Emergency stop

---

# 🧵 9. Concurrency Requirements

* CAN RX → **non-blocking, high priority**
* Control loop → deterministic (std::chrono or RT scheduling)
* No blocking calls inside control path

---

# 🧪 10. Testing Requirements

* Unit tests:

  * Frame parsing
  * Scaling correctness
* Integration:

  * Simulated CAN frames
* Fault injection:

  * Drop BCL
  * Inject invalid frames
* Long-run stability:

  * 48+ hour continuous operation

---

# 📦 11. Deliverables Expected

* Complete **C++ source code**
* **CMakeLists.txt**
* **Systemd service file**
* Config file (JSON/YAML)
* Logging framework
* Sample CAN traces

---

# 🚀 12. Output Expectations from Model

The model must generate:

1. Full **modular C++ implementation**
2. **GB/T state machine (complete)**
3. **CAN interface (SocketCAN)**
4. **Power module abstraction**
5. **Scheduler + watchdog**
6. **OCPP client skeleton**
7. Example configs + build system

---

# 💡 13. Constraints

* No monolithic loops
* Strict separation of:

  * Protocol
  * Hardware
  * Business logic
* Must support future:

  * CCS integration
  * EMS/load balancing
  * OTA updates