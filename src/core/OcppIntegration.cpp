#include "OcppIntegration.hpp"

#include "../utils/Logger.hpp"

namespace core {

OcppIntegration::OcppIntegration(std::shared_ptr<ocpp::OcppClient> client)
    : m_client(std::move(client)) {}

void OcppIntegration::start() {
  if (m_client) {
    m_client->start();
  }
}

void OcppIntegration::stop() {
  if (m_client) {
    m_client->stop();
  }
}

void OcppIntegration::tick() {
  if (!m_client) {
    return;
  }

  // Send dummy meter values.
  m_client->sendMeterValues(gbt::constants::kDummyVoltage,
                            gbt::constants::kDummyCurrent,
                            gbt::constants::kDummyEnergyKWh);
}

} // namespace core
