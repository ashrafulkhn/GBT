#pragma once

#include "GbtFrames.hpp"

#include <optional>

namespace gbt {

/// High-level helper for building GB/T frames and interpreting incoming frames.
class GbtController {
public:
  /// Create a BCL (control command) from vehicle demand.
  ///
  /// @param voltage_dV Target voltage in 0.1V units.
  /// @param current_dA Target current in 0.1A units.
  /// @param mode Charging mode value.
  static GbtFrame makeBcl(uint16_t voltage_dV, uint16_t current_dA, uint16_t mode = 0);

  /// Create a CCS (charger status) frame.
  ///
  /// @param voltage_dV Measured output voltage (0.1V).
  /// @param current_dA Measured output current (0.1A).
  /// @param status Charger status byte.
  /// @param error Error code.
  static GbtFrame makeCcs(uint16_t voltage_dV,
                          uint16_t current_dA,
                          uint8_t status,
                          uint8_t error);

  /// Create a BCP (charging parameters) frame.
  static GbtFrame makeBcp(uint16_t maxVoltage_dV,
                          uint16_t maxCurrent_dA,
                          uint16_t energy_dWh,
                          uint8_t soc);

  /// Attempt to parse a received CAN frame (must be DLC=8).
  static std::optional<GbtFrame> parseFrame(uint32_t canId,
                                           const uint8_t *payload,
                                           size_t dlc);
};

} // namespace gbt
