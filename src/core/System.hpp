#pragma once

#include "../can/CanInterface.hpp"
#include "../gbt/GbtStateMachine.hpp"
#include "../power/PowerModule.hpp"

#include <memory>
#include <optional>

namespace core {

/// High-level system that ties the GB/T state machine to CAN transport.
class System {
public:
  explicit System(std::shared_ptr<can::CanInterface> can,
                  std::shared_ptr<power::PowerModule> powerModule);

  /// Run one iteration: read any incoming frame, update state, and send response.
  ///
  /// @param timeoutMs How long to block waiting for incoming CAN frames.
  bool tick(uint32_t timeoutMs = 50);

  gbt::GbtStateMachine::State state() const;
  gbt::GbtStateMachine::Demand demand() const;

  void setLimits(uint16_t maxVoltage_dV, uint16_t maxCurrent_dA);

private:
  void updatePowerModule();

  std::shared_ptr<can::CanInterface> m_can;
  std::shared_ptr<power::PowerModule> m_powerModule;
  gbt::GbtStateMachine m_stateMachine;
  gbt::GbtStateMachine::State m_lastState;
};

} // namespace core
