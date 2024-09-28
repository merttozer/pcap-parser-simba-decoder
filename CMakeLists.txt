# CMake version requirement
cmake_minimum_required(VERSION 3.10)

# Project name
project(SimbaPacketAnalyzer)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories (header files)
include_directories(include)

# Set build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Add debug flags
set(CMAKE_CXX_FLAGS_DEBUG "-g")

# Add the executable
add_executable(pcap_parser
    src/main.cpp
    src/pcap_parser.cpp
    src/simba_decoder.cpp
    src/thread_pool.cpp
)

# Specify the output directory for the build
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
