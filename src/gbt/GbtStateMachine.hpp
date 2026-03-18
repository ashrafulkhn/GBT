#pragma once

#include "GbtController.hpp"

#include <optional>

namespace gbt {

/// Simplified GB/T state machine for Phase 1.
///
/// This state machine focuses on parsing BCL (EV → Charger) and responding with
/// CCS (Charger → EV) while maintaining a simple charger state.
class GbtStateMachine {
public:
  enum class State {
    Idle,
    Ready,
    Charging,
    Stopped,
    Fault,
  };

  struct Demand {
    uint16_t voltage_dV = 0; // 0.1 V units
    uint16_t current_dA = 0; // 0.1 A units
    uint16_t mode = 0;
  };

  GbtStateMachine();

  /// Process an incoming frame from the EV.
  void onFrame(const GbtFrame &frame);

  /// Should be called periodically to generate an outgoing frame (if any).
  ///
  /// This may return a frame to be sent to the EV (e.g., BCP/CCS).
  std::optional<GbtFrame> nextFrame();

  State state() const noexcept { return m_state; }
  Demand demand() const noexcept { return m_demand; }

  /// Configure charger limits (in 0.1 units).
  void setLimits(uint16_t maxVoltage_dV, uint16_t maxCurrent_dA);

private:
  State m_state;
  Demand m_demand;
  uint16_t m_maxVoltage_dV;
  uint16_t m_maxCurrent_dA;
  uint16_t m_lastSentCount;
};

} // namespace gbt
