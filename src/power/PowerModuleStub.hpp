#pragma once

#include "PowerModule.hpp"

namespace power {

/// A lightweight stub implementation that does nothing.
///
/// Useful for early testing and builds where the real power module code is not
/// yet provided.
class PowerModuleStub : public PowerModule {
public:
  void startModule(uint16_t /*voltage_dV*/, uint16_t /*current_dA*/, gbt::GbtStateMachine::State /*state*/) override {
    // no-op
  }

  void stopModule() override {
    // no-op
  }
};

} // namespace power
