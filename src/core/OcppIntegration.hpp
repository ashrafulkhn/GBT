#pragma once

#include "../ocpp/OcppClient.hpp"
#include "../utils/Constants.hpp"

#include <memory>

namespace core {

/// Lightweight OCPP integration.
///
/// Currently this module sends dummy meter values at a fixed interval.
class OcppIntegration {
public:
  explicit OcppIntegration(std::shared_ptr<ocpp::OcppClient> client);

  void start();
  void stop();

  /// Send dummy meter values on each tick.
  void tick();

private:
  std::shared_ptr<ocpp::OcppClient> m_client;
};

} // namespace core
