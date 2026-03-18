#include "can/DummyCan.hpp"
#include "can/SocketCan.hpp"
#include "core/OcppIntegration.hpp"
#include "core/System.hpp"
#include "ocpp/OcppClient.hpp"
#include "power/PowerModuleStub.hpp"
#include "utils/Logger.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

static void printUsage(const char *progName) {
  std::cout << "Usage: " << progName << " <command> [args]\n";
  std::cout << "Commands:\n";
  std::cout << "  run <can-iface>   - run the GB/T controller loop on the given CAN interface\n";
  std::cout << "  dump <can-id> <8-bytes-hex> - parse a single GB/T frame\n";
}

static std::optional<std::array<uint8_t, 8>> parseHex8(const std::string &hex) {
  std::string s;
  for (char c : hex) {
    if (!isspace(static_cast<unsigned char>(c))) {
      s.push_back(c);
    }
  }
  if (s.size() != 16) {
    return std::nullopt;
  }

  std::array<uint8_t, 8> out{};
  for (size_t i = 0; i < 8; ++i) {
    const auto byteStr = s.substr(i * 2, 2);
    out[i] = static_cast<uint8_t>(std::strtoul(byteStr.c_str(), nullptr, 16));
  }
  return out;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printUsage(argv[0]);
    return 1;
  }

  const std::string cmd = argv[1];

  if (cmd == "run") {
    if (argc != 3) {
      printUsage(argv[0]);
      return 1;
    }
    const std::string canIface = argv[2];

    std::shared_ptr<can::CanInterface> can = std::make_shared<can::SocketCan>(canIface);
    if (auto socketCan = std::dynamic_pointer_cast<can::SocketCan>(can)) {
      if (!socketCan->isOk()) {
        gbt::warn("Failed to open CAN interface: " + canIface + ". Falling back to dummy CAN.");
        can = std::make_shared<can::DummyCan>();
      }
    }

    // Power module stub; replace with real implementation.
    auto powerModule = std::make_shared<power::PowerModuleStub>();
    core::System system(can, powerModule);

    // OCPP client (dummy implementation for now).
    auto ocppClient = std::make_shared<ocpp::OcppClient>("ws://localhost:8080/ocpp");
    core::OcppIntegration ocppIntegration(ocppClient);
    ocppIntegration.start();

    gbt::info("Starting GB/T controller on " + canIface);
    while (true) {
      if (!system.tick(100)) {
        break;
      }
      ocppIntegration.tick();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    ocppIntegration.stop();
    return 0;
  }

  if (cmd == "dump") {
    if (argc != 4) {
      printUsage(argv[0]);
      return 1;
    }

    const uint32_t canId = static_cast<uint32_t>(std::strtoul(argv[2], nullptr, 0));
    auto bytes = parseHex8(argv[3]);
    if (!bytes) {
      gbt::error("Expected 8 bytes of hex data (16 chars)");
      return 1;
    }

    auto frame = gbt::GbtController::parseFrame(canId, bytes->data(), bytes->size());
    if (!frame) {
      gbt::error("Failed to parse GB/T frame");
      return 1;
    }

    gbt::info("Parsed GB/T frame:");
    std::cout << "  id=0x" << std::hex << frame->id << std::dec << "\n";
    std::cout << "  data=";
    for (auto b : frame->data) {
      std::cout << std::hex << static_cast<int>(b) << " ";
    }
    std::cout << std::dec << "\n";
    return 0;
  }

  printUsage(argv[0]);
  return 1;
}
