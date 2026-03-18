#include "GbtFrames.hpp"

#include <cstring>

namespace gbt {

GbtFrame GbtFrame::makeBCL(uint16_t voltage_dV, uint16_t current_dA, uint16_t mode) {
  GbtFrame f;
  f.id = can_id::BCL;
  // Little endian field encoding as in spec.
  f.data[0] = static_cast<uint8_t>(voltage_dV & 0xFF);
  f.data[1] = static_cast<uint8_t>((voltage_dV >> 8) & 0xFF);
  f.data[2] = static_cast<uint8_t>(current_dA & 0xFF);
  f.data[3] = static_cast<uint8_t>((current_dA >> 8) & 0xFF);
  f.data[4] = static_cast<uint8_t>(mode & 0xFF);
  f.data[5] = static_cast<uint8_t>((mode >> 8) & 0xFF);
  // reserved bytes
  f.data[6] = 0;
  f.data[7] = 0;
  return f;
}

GbtFrame GbtFrame::makeCCS(uint16_t voltage_dV, uint16_t current_dA, uint8_t status, uint8_t error) {
  GbtFrame f;
  f.id = can_id::CCS;
  f.data[0] = static_cast<uint8_t>(voltage_dV & 0xFF);
  f.data[1] = static_cast<uint8_t>((voltage_dV >> 8) & 0xFF);
  f.data[2] = static_cast<uint8_t>(current_dA & 0xFF);
  f.data[3] = static_cast<uint8_t>((current_dA >> 8) & 0xFF);
  f.data[4] = status;
  f.data[5] = error;
  f.data[6] = 0;
  f.data[7] = 0;
  return f;
}

GbtFrame GbtFrame::makeBCP(uint16_t maxVoltage_dV, uint16_t maxCurrent_dA, uint16_t energy_dWh, uint8_t soc) {
  GbtFrame f;
  f.id = can_id::BCP;
  f.data[0] = static_cast<uint8_t>(maxVoltage_dV & 0xFF);
  f.data[1] = static_cast<uint8_t>((maxVoltage_dV >> 8) & 0xFF);
  f.data[2] = static_cast<uint8_t>(maxCurrent_dA & 0xFF);
  f.data[3] = static_cast<uint8_t>((maxCurrent_dA >> 8) & 0xFF);
  f.data[4] = static_cast<uint8_t>(energy_dWh & 0xFF);
  f.data[5] = static_cast<uint8_t>((energy_dWh >> 8) & 0xFF);
  f.data[6] = soc;
  f.data[7] = 0;
  return f;
}

std::optional<GbtFrame> GbtFrame::parse(uint32_t canId, const uint8_t *payload, size_t dlc) {
  if (dlc != 8 || payload == nullptr) {
    return std::nullopt;
  }
  GbtFrame f;
  f.id = canId;
  std::memcpy(f.data.data(), payload, 8);
  return f;
}

} // namespace gbt
