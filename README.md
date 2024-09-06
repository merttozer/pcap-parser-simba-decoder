# PCAP SIMBA Decoder

This project is a PCAP parser and SIMBA protocol decoder implemented in C++. It reads network packets from a `.pcap` file, decodes the payload using the SIMBA protocol, and outputs the decoded data to a specified file in JSON format.

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Getting Started](#getting-started)
- [Installation](#installation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Classes and Structs](#classes-and-structs)
- [Contributing](#contributing)
- [License](#license)

## Overview

The PCAP SIMBA Decoder project allows you to parse network capture files (`.pcap`) and decode the payload data encoded using the SIMBA protocol. The decoded data is then saved as a JSON file, making it easy to analyze and visualize.

## Features

- **PCAP Parsing**: Parse `.pcap` files to extract individual network packets.
- **Protocol Decoding**: Decode payload data using the SIMBA protocol.
- **Output to JSON**: Save decoded data in a human-readable JSON format.
- **Cross-Platform**: Compatible with major operating systems (Linux, Windows, macOS).

## Getting Started

This section will guide you through the steps to set up and run the PCAP SIMBA Decoder on your local machine.

### Prerequisites

Before you begin, ensure you have the following installed on your system:

- **C++ Compiler**: A modern C++ compiler that supports C++11 or later (e.g., GCC, Clang, MSVC).
- **CMake**: A cross-platform build system generator, which is used to build the project.
- **Git**: Version control system to clone the repository (optional if you download the source code directly).

### Installation

1. **Clone the Repository**  
   Clone the project repository to your local machine using Git:

   ```bash
   git clone https://github.com/merttozer/pcap-simba-decoder.git
   cd pcap-simba-decoder
   ```

2. **Build the Project**  
   Use CMake to configure and build the project:

   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```
   This will generate the executable for the PCAP SIMBA Decoder.

3. **Run the Application**  
   Once the build is complete, you can run the decoder using the following command:

   ```bash
   ./pcap_decoder <path-to-pcap-file> <output-json-file>
   ```
   Replace <path-to-pcap-file> with the path to your .pcap file and <output-json-file> with the desired output file name like below. This will parse the provided .pcap file, decode the SIMBA protocol data, and save the results to the specified JSON file.
   
    ```bash
   ./pcap_parser ../pcap_files/input.pcap ../output_files/output.json
   ```

## Project Structure

The project is organized into several key components:

- **src/**: This directory contains the main source files of the project.
  - `main.cpp`: The entry point of the application. It initializes the `PcapParser` and starts the parsing and decoding process.
  - `pcap_parser.cpp`: Implements the `PcapParser` class, responsible for reading the PCAP file, parsing its headers, and processing the captured packets.
  - `simba_decoder.cpp`: Implements the `SimbaDecoder` class, which decodes the SIMBA protocol data extracted from the packets.
- **include/**: This directory contains the header files corresponding to the source files.
  - `pcap_parser.hpp`: Declares the `PcapParser` class and its methods.
  - `pcap_messages.hpp`: Defines the data structures used for PCAP, Ethernet, IP, and UDP headers, as well as the structure for holding a complete packet.
  - `simba_decoder.hpp`: Declares the `SimbaDecoder` class and its methods.
  - `simba_messages.hpp`: Defines the data structures used for the SIMBA protocol messages and associated fields.
- **build/**: This directory is where the compiled binaries and other build artifacts will be stored after running the build commands.
- **CMakeLists.txt**: The CMake configuration file that defines how the project is built, including source files, include directories, and compiler options.

This structure keeps the project modular and maintainable, making it easier to navigate and understand.

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request for any improvements or bug fixes.

## License

This project is licensed under the MIT License. See the LICENSE file for details.