#include "GbtStateMachine.hpp"

namespace gbt {

GbtStateMachine::GbtStateMachine()
    : m_state(State::Idle), m_maxVoltage_dV(1000), m_maxCurrent_dA(3000), m_lastSentCount(0) {}

void GbtStateMachine::setLimits(uint16_t maxVoltage_dV, uint16_t maxCurrent_dA) {
  m_maxVoltage_dV = maxVoltage_dV;
  m_maxCurrent_dA = maxCurrent_dA;
}

void GbtStateMachine::onFrame(const GbtFrame &frame) {
  switch (frame.id) {
  case can_id::BCL: {
    // Parse demand from EV.
    const uint16_t vol = static_cast<uint16_t>(frame.data[0] | (frame.data[1] << 8));
    const uint16_t cur = static_cast<uint16_t>(frame.data[2] | (frame.data[3] << 8));
    const uint16_t mode = static_cast<uint16_t>(frame.data[4] | (frame.data[5] << 8));

    m_demand.voltage_dV = vol;
    m_demand.current_dA = cur;
    m_demand.mode = mode;

    if (mode == 0) {
      m_state = State::Charging;
    } else {
      m_state = State::Stopped;
    }
    break;
  }

  case can_id::BCP:
    // EV is asking for parameters; respond with readiness.
    m_state = State::Ready;
    break;

  default:
    break;
  }
}

std::optional<GbtFrame> GbtStateMachine::nextFrame() {
  // Emit BCP from Idle/Ready states.
  if (m_state == State::Idle || m_state == State::Ready) {
    return GbtController::makeBcp(m_maxVoltage_dV, m_maxCurrent_dA, 0, 0);
  }

  // If charging, respond with status.
  if (m_state == State::Charging) {
    // Keep output limited to configured maximums.
    const uint16_t outV = std::min(m_demand.voltage_dV, m_maxVoltage_dV);
    const uint16_t outI = std::min(m_demand.current_dA, m_maxCurrent_dA);
    return GbtController::makeCcs(outV, outI, /*status=*/1, /*error=*/0);
  }

  // If stopped, send status indicating stopped.
  if (m_state == State::Stopped) {
    return GbtController::makeCcs(0, 0, /*status=*/0, /*error=*/0);
  }

  return std::nullopt;
}

} // namespace gbt
