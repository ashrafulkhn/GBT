#pragma once

#include <iostream>
#include <string>

namespace gbt {

enum class LogLevel { Info, Warn, Error };

inline void log(LogLevel level, const std::string &msg) {
  const char *prefix = "[INFO]";
  switch (level) {
  case LogLevel::Info:
    prefix = "[INFO]";
    break;
  case LogLevel::Warn:
    prefix = "[WARN]";
    break;
  case LogLevel::Error:
    prefix = "[ERROR]";
    break;
  }
  std::cout << prefix << " " << msg << "\n";
}

inline void info(const std::string &msg) { log(LogLevel::Info, msg); }
inline void warn(const std::string &msg) { log(LogLevel::Warn, msg); }
inline void error(const std::string &msg) { log(LogLevel::Error, msg); }

} // namespace gbt
