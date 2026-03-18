#include "OcppClient.hpp"

#include "../utils/Logger.hpp"

namespace ocpp {

OcppClient::OcppClient(std::string csmsUrl)
    : m_csmsUrl(std::move(csmsUrl)), m_running(false) {}

OcppClient::~OcppClient() { stop(); }

bool OcppClient::start() {
  if (m_running) {
    return true;
  }
  gbt::info("OCPP client starting, CSMS: " + m_csmsUrl);
  // In a real client: open WebSocket to CSMS and perform handshake.
  m_running = true;
  return true;
}

void OcppClient::stop() {
  if (!m_running) {
    return;
  }
  gbt::info("OCPP client stopping");
  m_running = false;
}

void OcppClient::sendStatus(const std::string &status) {
  if (!m_running) {
    return;
  }
  gbt::info("OCPP: StatusNotification -> " + status);
}

void OcppClient::sendMeterValues(float voltage, float current, float energy_kwh) {
  if (!m_running) {
    return;
  }
  gbt::info("OCPP: MeterValues -> V=" + std::to_string(voltage) +
            " A=" + std::to_string(current) +
            " kWh=" + std::to_string(energy_kwh));
}

} // namespace ocpp
