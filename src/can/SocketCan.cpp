#include "SocketCan.hpp"

#include <chrono>
#include <cstring>
#include <deque>
#include <mutex>

#ifdef __linux__
#  include <fcntl.h>
#  include <linux/can.h>
#  include <linux/can/raw.h>
#  include <net/if.h>
#  include <sys/ioctl.h>
#  include <sys/socket.h>
#  include <unistd.h>
#endif

namespace can {

#ifdef __linux__

static int openCanSocket(const std::string &ifname, uint32_t /*filterId*/) {
  int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if (s < 0) {
    return -1;
  }

  struct ifreq ifr;
  std::strncpy(ifr.ifr_name, ifname.c_str(), IFNAMSIZ - 1);
  ifr.ifr_name[IFNAMSIZ - 1] = '\0';

  if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
    close(s);
    return -1;
  }

  struct sockaddr_can addr{};
  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;

  if (bind(s, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0) {
    close(s);
    return -1;
  }

  // Non-blocking reads are useful for select/poll loops.
  fcntl(s, F_SETFL, O_NONBLOCK);

  return s;
}

SocketCan::SocketCan(std::string interfaceName, uint32_t canIdFilter)
    : m_socket(openCanSocket(interfaceName, canIdFilter)) {}

SocketCan::~SocketCan() {
  m_running = false;
  if (m_socket >= 0) {
    close(m_socket);
    m_socket = -1;
  }
}

bool SocketCan::send(const Frame &frame) {
  if (m_socket < 0) {
    return false;
  }

  struct can_frame cf{};
  cf.can_id = frame.id & CAN_SFF_MASK;
  cf.can_dlc = static_cast<__u8>(std::min<size_t>(frame.data.size(), 8));
  std::memcpy(cf.data, frame.data.data(), cf.can_dlc);

  const ssize_t n = write(m_socket, &cf, sizeof(cf));
  return n == sizeof(cf);
}

std::optional<Frame> SocketCan::receive(uint32_t timeoutMs) {
  if (m_socket < 0) {
    return std::nullopt;
  }

  struct can_frame cf{};
  if (timeoutMs > 0) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(m_socket, &fds);

    struct timeval tv;
    tv.tv_sec = timeoutMs / 1000;
    tv.tv_usec = (timeoutMs % 1000) * 1000;

    const int ret = select(m_socket + 1, &fds, nullptr, nullptr, &tv);
    if (ret <= 0) {
      return std::nullopt;
    }
  }

  const ssize_t n = read(m_socket, &cf, sizeof(cf));
  if (n != sizeof(cf)) {
    return std::nullopt;
  }

  Frame frame;
  frame.id = cf.can_id & CAN_SFF_MASK;
  frame.data.assign(cf.data, cf.data + cf.can_dlc);
  return frame;
}

#else

SocketCan::SocketCan(std::string /*interfaceName*/, uint32_t /*canIdFilter*/) {}
SocketCan::~SocketCan() = default;

bool SocketCan::send(const Frame &) { return true; }

std::optional<Frame> SocketCan::receive(uint32_t /*timeoutMs*/) {
  return std::nullopt;
}

#endif

} // namespace can
