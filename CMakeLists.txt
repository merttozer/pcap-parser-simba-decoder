# CMake version requirement
cmake_minimum_required(VERSION 3.10)

# Project name
project(SimbaPacketAnalyzer)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories (header files)
include_directories(include)

# Add the executable
add_executable(pcap_parser
    src/main.cpp
    src/pcap_parser.cpp
    src/simba_decoder.cpp
)

# Specify the output directory for the build
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
