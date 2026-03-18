#pragma once

namespace gbt {

/// Dummy meter values for OCPP reporting.
///
/// These constants are used by the OCPP client to simulate a real meter.
namespace constants {

static constexpr float kDummyVoltage = 400.0f;   // volts
static constexpr float kDummyCurrent = 32.0f;    // amps
static constexpr float kDummyEnergyKWh = 0.5f;   // kWh

} // namespace constants

} // namespace gbt
