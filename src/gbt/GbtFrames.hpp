#pragma once

#include <array>
#include <cstdint>
#include <optional>

namespace gbt {

/// CAN IDs used by GB/T 27930 (DC charging).
namespace can_id {
static constexpr uint32_t BCL = 0x1810F456; // Control Command (EV→Charger)
static constexpr uint32_t CCS = 0x1811F456; // Charger Status (Charger→EV)
static constexpr uint32_t BCP = 0x1806F456; // Charging Parameters

// Other frames (handshake, identification, etc.) may use different IDs.
} // namespace can_id

/// Fixed-size 8-byte GB/T data payload.
struct GbtFrame {
  uint32_t id = 0;
  std::array<uint8_t, 8> data{};

  static GbtFrame makeBCL(uint16_t voltage_dV, uint16_t current_dA, uint16_t mode = 0);
  static GbtFrame makeCCS(uint16_t voltage_dV, uint16_t current_dA, uint8_t status, uint8_t error);
  static GbtFrame makeBCP(uint16_t maxVoltage_dV, uint16_t maxCurrent_dA, uint16_t energy_dWh, uint8_t soc);

  /// Parse a raw CAN frame (must be DLC=8)
  static std::optional<GbtFrame> parse(uint32_t canId, const uint8_t *payload, size_t dlc);
};

} // namespace gbt
