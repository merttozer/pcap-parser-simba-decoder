#include "../include/pcap_parser.hpp"
#include "../include/simba_decoder.hpp"
#include <iomanip> // For std::setw and std::setfill
#include <iostream>
#include <arpa/inet.h> // For network byte order functions (Linux/Unix systems)

namespace pcap {

// Constructor initializes the input and output filenames
PcapParser::PcapParser(const std::string& filename,
                       const std::string& outputFile)
  : filename(filename)
  , outputFile(outputFile)
  , globalHeader{}
{
}

// Main parse function to process the pcap file
void PcapParser::parse()
{
    std::ifstream pcapFile(filename, std::ios::binary);
    if (!pcapFile) {
        throw std::runtime_error("Error: Could not open pcap file.");
    }

    std::ofstream outFile(outputFile);
    if (!outFile.is_open()) {
        throw std::runtime_error("Error: Could not open output file.");
    }

    // Parse the global header
    globalHeader = parseGlobalHeader(pcapFile);

    // Read and parse packets until the end of the file
    while (pcapFile.peek() != EOF) {
        PcapPacket packet = parsePacket(pcapFile);
        saveDecodedPacket(packet, outFile);
    }

    outFile.close();
    pcapFile.close();
}

// Parse the global header from the pcap file
PcapGlobalHeader PcapParser::parseGlobalHeader(std::ifstream& file)
{
    PcapGlobalHeader header;
    file.read(reinterpret_cast<char*>(&header), PcapGlobalHeader::SIZE);
    if (!file) {
        throw std::runtime_error("Error reading global header.");
    }
    return header;
}

// Parse a single packet from the pcap file
PcapPacket PcapParser::parsePacket(std::ifstream& file)
{
    PcapPacket packet;

    // Read the packet header
    file.read(reinterpret_cast<char*>(&packet.header), PcapPacketHeader::SIZE);
    if (!file) {
        throw std::runtime_error("Error reading packet header.");
    }

    // Parse the Ethernet, IPv4, and UDP headers
    packet.ethernetHeader = parseEthernetHeader(file);
    packet.ipHeader = parseIPv4Header(file);
    packet.udpHeader = parseUDPHeader(file);

    // Calculate the total length of headers
    size_t ipHeaderSize = (packet.ipHeader.versionAndHeaderLength & 0x0F) * 4;
    size_t headersLength =
      EthernetHeader::SIZE + ipHeaderSize + UDPHeader::SIZE;

    // Read and store the payload (SIMBA data)
    size_t payloadSize = packet.header.incl_len - headersLength;
    packet.data.resize(payloadSize);
    file.read(reinterpret_cast<char*>(packet.data.data()), payloadSize);
    if (!file) {
        throw std::runtime_error("Error reading packet data.");
    }

    return packet;
}

// Parse the Ethernet header from the packet
EthernetHeader PcapParser::parseEthernetHeader(std::ifstream& file)
{
    EthernetHeader ethernetHeader;
    file.read(reinterpret_cast<char*>(&ethernetHeader), EthernetHeader::SIZE);
    if (!file) {
        throw std::runtime_error("Error reading Ethernet header.");
    }
    return ethernetHeader;
}

// Parse the IPv4 header from the packet
IPv4Header PcapParser::parseIPv4Header(std::ifstream& file)
{
    IPv4Header ipHeader;
    file.read(reinterpret_cast<char*>(&ipHeader),
              IPv4Header::BASE_HEADER_SIZE); // Read the first 20 bytes

    uint8_t ihl = ipHeader.versionAndHeaderLength & 0x0F; // Get the IHL value
    size_t ipHeaderSize = ihl * 4;

    // Read any additional options in the extended header
    if (ipHeaderSize > IPv4Header::BASE_HEADER_SIZE) {
        file.read(reinterpret_cast<char*>(&ipHeader) +
                    IPv4Header::BASE_HEADER_SIZE,
                  ipHeaderSize - IPv4Header::BASE_HEADER_SIZE);
        if (!file) {
            throw std::runtime_error("Error reading extended IPv4 header.");
        }
    }
    return ipHeader;
}

// Parse the UDP header from the packet
UDPHeader PcapParser::parseUDPHeader(std::ifstream& file)
{
    UDPHeader udpHeader;
    file.read(reinterpret_cast<char*>(&udpHeader), UDPHeader::SIZE);
    if (!file) {
        throw std::runtime_error("Error reading UDP header.");
    }
    return udpHeader;
}

// Save the decoded packet as JSON to the output file
void PcapParser::saveDecodedPacket(const PcapPacket& packet,
                                   std::ofstream& outFile)
{
    simba::SimbaDecoder decoder(packet.data);
    decoder.decode();
    std::string jsonOutput = decoder.toJSON();
    outFile << jsonOutput << std::endl;
}

} // namespace pcap
