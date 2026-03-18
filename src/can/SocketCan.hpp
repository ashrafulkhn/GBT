#pragma once

#include "CanInterface.hpp"

#include <atomic>
#include <string>

namespace can {

/// SocketCAN implementation of CanInterface.
///
/// This class uses Linux SocketCAN (PF_CAN/SOCK_RAW) and is intended for
/// Linux targets (Yocto, Debian, etc.).
class SocketCan : public CanInterface {
public:
  /// @param interfaceName CAN interface (e.g., "can0").
  /// @param canIdFilter Optional CAN ID to filter (0 = no filter).
  explicit SocketCan(std::string interfaceName = "can0", uint32_t canIdFilter = 0);
  ~SocketCan() override;

  bool send(const Frame &frame) override;
  std::optional<Frame> receive(uint32_t timeoutMs = 0) override;

  bool isOk() const noexcept { return m_socket >= 0; }

private:
  int m_socket = -1;
  std::atomic<bool> m_running{true};
};

} // namespace can
