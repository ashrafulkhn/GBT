#pragma once

#include <string>

namespace ocpp {

/// Minimal OCPP client stub.
///
/// In a production system this would implement OCPP 1.6J over WebSockets.
/// For Phase 3, this implementation simply logs the messages.
class OcppClient {
public:
  explicit OcppClient(std::string csmsUrl);
  ~OcppClient();

  /// Start the client (connect to CSMS).
  bool start();

  /// Stop the client.
  void stop();

  /// Send a status notification.
  void sendStatus(const std::string &status);

  /// Send meter values over OCPP.
  void sendMeterValues(float voltage, float current, float energy_kwh);

private:
  std::string m_csmsUrl;
  bool m_running;
};

} // namespace ocpp
