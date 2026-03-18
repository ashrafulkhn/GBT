#include "System.hpp"

#include "../utils/Logger.hpp"

namespace core {

System::System(std::shared_ptr<can::CanInterface> can,
               std::shared_ptr<power::PowerModule> powerModule)
    : m_can(std::move(can)),
      m_powerModule(std::move(powerModule)),
      m_lastState(gbt::GbtStateMachine::State::Idle) {
  m_stateMachine.setLimits(1000, 3000);
}

bool System::tick(uint32_t timeoutMs) {
  if (!m_can) {
    gbt::error("CAN transport not initialized.");
    return false;
  }

  // Read one frame (if available).
  auto frame = m_can->receive(timeoutMs);
  if (frame) {
    gbt::info("CAN frame received (id=0x" + std::to_string(frame->id) + ")");

    auto gbtFrame = gbt::GbtController::parseFrame(frame->id, frame->data.data(), frame->data.size());
    if (gbtFrame) {
      m_stateMachine.onFrame(*gbtFrame);
    } else {
      gbt::warn("Received CAN frame could not be interpreted as GB/T message.");
    }
  }

  // If GB/T state changed, update power module.
  if (m_stateMachine.state() != m_lastState) {
    gbt::info("GB/T state changed: " + std::to_string(static_cast<int>(m_lastState)) + " -> " +
              std::to_string(static_cast<int>(m_stateMachine.state())));
    m_lastState = m_stateMachine.state();
  }

  updatePowerModule();

  // Determine next outgoing frame.
  const auto outgoing = m_stateMachine.nextFrame();
  if (outgoing) {
    can::Frame canFrame;
    canFrame.id = outgoing->id;
    canFrame.data.assign(outgoing->data.begin(), outgoing->data.end());
    if (!m_can->send(canFrame)) {
      gbt::error("Failed to send CAN frame.");
      return false;
    }
  }

  return true;
}

void System::updatePowerModule() {
  if (!m_powerModule) {
    return;
  }

  const auto state = m_stateMachine.state();
  const auto demand = m_stateMachine.demand();

  switch (state) {
  case gbt::GbtStateMachine::State::Charging:
    m_powerModule->startModule(demand.voltage_dV, demand.current_dA, state);
    break;

  case gbt::GbtStateMachine::State::Stopped:
  case gbt::GbtStateMachine::State::Fault:
  case gbt::GbtStateMachine::State::Idle:
  case gbt::GbtStateMachine::State::Ready:
  default:
    m_powerModule->stopModule();
    break;
  }
}

gbt::GbtStateMachine::State System::state() const { return m_stateMachine.state(); }

gbt::GbtStateMachine::Demand System::demand() const { return m_stateMachine.demand(); }

void System::setLimits(uint16_t maxVoltage_dV, uint16_t maxCurrent_dA) {
  m_stateMachine.setLimits(maxVoltage_dV, maxCurrent_dA);
}

} // namespace core
