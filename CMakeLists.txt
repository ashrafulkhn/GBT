cmake_minimum_required(VERSION 3.15)
project(GBTCharger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Core library containing GB/T protocol and CAN transport.
add_library(gbt_core
  src/gbt/GbtFrames.cpp
  src/gbt/GbtController.cpp
  src/gbt/GbtStateMachine.cpp
  src/can/SocketCan.cpp
  src/can/DummyCan.cpp
  src/core/System.cpp
  src/core/OcppIntegration.cpp
  src/ocpp/OcppClient.cpp
  src/utils/Logger.cpp
)

target_include_directories(gbt_core PUBLIC src)

target_compile_features(gbt_core PUBLIC cxx_std_17)

# Warnings
if(MSVC)
  target_compile_options(gbt_core PRIVATE /W4 /permissive-)
else()
  target_compile_options(gbt_core PRIVATE -Wall -Wextra -Wpedantic -Wconversion)
endif()

add_executable(gbt src/main.cpp)
add_dependencies(gbt gbt_core)

target_link_libraries(gbt PRIVATE gbt_core)

target_include_directories(gbt PRIVATE src)
