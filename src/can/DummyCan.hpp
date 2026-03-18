#pragma once

#include "CanInterface.hpp"

#include <condition_variable>
#include <mutex>
#include <queue>

namespace can {

/// Simple in-memory CAN transport for testing.
class DummyCan : public CanInterface {
public:
  DummyCan() = default;
  ~DummyCan() override = default;

  bool send(const Frame &frame) override;
  std::optional<Frame> receive(uint32_t timeoutMs = 0) override;

private:
  std::mutex m_mutex;
  std::condition_variable m_event;
  std::queue<Frame> m_queue;
};

} // namespace can
