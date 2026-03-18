#pragma once

#include <cstdint>
#include <optional>
#include <vector>

namespace can {

struct Frame {
  uint32_t id = 0;
  std::vector<uint8_t> data;
};

/// Abstract CAN transport.
class CanInterface {
public:
  virtual ~CanInterface() = default;

  /// Send a frame over the bus.
  virtual bool send(const Frame &frame) = 0;

  /// Receive a frame. Returns nullopt if no frame is available within timeout.
  virtual std::optional<Frame> receive(uint32_t timeoutMs = 0) = 0;
};

} // namespace can
