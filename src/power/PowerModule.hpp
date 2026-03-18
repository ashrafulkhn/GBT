#pragma once

#include "../gbt/GbtStateMachine.hpp"

#include <cstdint>

namespace power {

/// Interface for a power stage controller.
///
/// Implementations should drive the hardware (power modules) to achieve the
/// requested voltage/current targeted by the GB/T charging protocol.
class PowerModule {
public:
  virtual ~PowerModule() = default;

  /// Called when the GB/T controller has a new target and/or state.
  ///
  /// @param voltage_dV Target output voltage in 0.1V units.
  /// @param current_dA Target output current in 0.1A units.
  /// @param state Current GB/T state (Charging/Stopped/etc.)
  virtual void startModule(uint16_t voltage_dV,
                           uint16_t current_dA,
                           gbt::GbtStateMachine::State state) = 0;

  /// Called when charging is stopped or a fault occurs.
  virtual void stopModule() = 0;
};

} // namespace power
