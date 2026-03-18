#include "DummyCan.hpp"

namespace can {

bool DummyCan::send(const Frame &frame) {
  {
    std::lock_guard lock(m_mutex);
    m_queue.push(frame);
    if (m_queue.size() > 128) {
      m_queue.pop();
    }
  }
  m_event.notify_one();
  return true;
}

std::optional<Frame> DummyCan::receive(uint32_t timeoutMs) {
  std::unique_lock lock(m_mutex);
  if (timeoutMs == 0) {
    if (m_queue.empty()) {
      return std::nullopt;
    }
  } else {
    if (!m_event.wait_for(lock, std::chrono::milliseconds(timeoutMs), [&] { return !m_queue.empty(); })) {
      return std::nullopt;
    }
  }

  if (m_queue.empty()) {
    return std::nullopt;
  }

  Frame frame = std::move(m_queue.front());
  m_queue.pop();
  return frame;
}

} // namespace can
