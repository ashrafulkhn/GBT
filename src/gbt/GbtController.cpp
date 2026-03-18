#include "GbtController.hpp"

namespace gbt {

GbtFrame GbtController::makeBcl(uint16_t voltage_dV, uint16_t current_dA, uint16_t mode) {
  return GbtFrame::makeBCL(voltage_dV, current_dA, mode);
}

GbtFrame GbtController::makeCcs(uint16_t voltage_dV,
                                uint16_t current_dA,
                                uint8_t status,
                                uint8_t error) {
  return GbtFrame::makeCCS(voltage_dV, current_dA, status, error);
}

GbtFrame GbtController::makeBcp(uint16_t maxVoltage_dV,
                                uint16_t maxCurrent_dA,
                                uint16_t energy_dWh,
                                uint8_t soc) {
  return GbtFrame::makeBCP(maxVoltage_dV, maxCurrent_dA, energy_dWh, soc);
}

std::optional<GbtFrame> GbtController::parseFrame(uint32_t canId,
                                                  const uint8_t *payload,
                                                  size_t dlc) {
  return GbtFrame::parse(canId, payload, dlc);
}

} // namespace gbt
