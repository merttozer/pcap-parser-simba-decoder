#ifndef PCAP_MESSAGES_HPP
#define PCAP_MESSAGES_HPP

#include <cstdint>
#include <vector>
#include <cstddef>
#include <type_traits>

#pragma pack(push, 1) // Ensure structures are packed to avoid padding

namespace pcap {

// Struct for the pcap global header
struct PcapGlobalHeader
{
    uint32_t magic_number;
    uint16_t version_major;
    uint16_t version_minor;
    int32_t thiszone;
    uint32_t sigfigs;
    uint32_t snaplen;
    uint32_t network;

    static constexpr size_t SIZE =
      sizeof(magic_number) + sizeof(version_major) + sizeof(version_minor) +
      sizeof(thiszone) + sizeof(sigfigs) + sizeof(snaplen) + sizeof(network);
};
static_assert(PcapGlobalHeader::SIZE == 24, "PcapGlobalHeader size mismatch!");

// Struct for the pcap packet header
struct PcapPacketHeader
{
    uint32_t ts_sec;   // Timestamp seconds
    uint32_t ts_usec;  // Timestamp microseconds
    uint32_t incl_len; // Number of octets of packet saved in file
    uint32_t orig_len; // Actual length of the packet

    static constexpr size_t SIZE =
      sizeof(ts_sec) + sizeof(ts_usec) + sizeof(incl_len) + sizeof(orig_len);
};
static_assert(PcapPacketHeader::SIZE == 16, "PcapPacketHeader size mismatch!");

// Struct for the Ethernet header
struct EthernetHeader
{
    uint8_t destination[6]; // Destination MAC address
    uint8_t source[6];      // Source MAC address
    uint16_t etherType;     // Protocol type

    static constexpr size_t SIZE =
      sizeof(destination) + sizeof(source) + sizeof(etherType);
};
static_assert(EthernetHeader::SIZE == 14, "EthernetHeader size mismatch!");

// Struct for the IPv4 header
struct IPv4Header
{
    uint8_t versionAndHeaderLength;  // Version and header length
    uint8_t typeOfService;           // Type of service
    uint16_t totalLength;            // Total length of the datagram
    uint16_t identification;         // Identification
    uint16_t flagsAndFragmentOffset; // Flags and fragment offset
    uint8_t ttl;                     // Time to live
    uint8_t protocol;                // Protocol
    uint16_t headerChecksum;         // Header checksum
    uint32_t sourceAddress;          // Source IP address
    uint32_t destinationAddress;     // Destination IP address

    static constexpr size_t BASE_HEADER_SIZE =
      20; // Minimum size for an IPv4 header
};
static_assert(sizeof(IPv4Header) == IPv4Header::BASE_HEADER_SIZE,
              "IPv4Header base size mismatch!");

// Struct for the UDP header
struct UDPHeader
{
    uint16_t sourcePort;      // Source port
    uint16_t destinationPort; // Destination port
    uint16_t length;          // Length of UDP header and data
    uint16_t checksum;        // Checksum

    static constexpr size_t SIZE = sizeof(sourcePort) +
                                   sizeof(destinationPort) + sizeof(length) +
                                   sizeof(checksum);
};
static_assert(UDPHeader::SIZE == 8, "UDPHeader size mismatch!");

// Struct to hold a packet, including its headers and data
struct PcapPacket
{
    PcapPacketHeader header;       // Packet header
    EthernetHeader ethernetHeader; // Ethernet header
    IPv4Header ipHeader;           // IPv4 header
    UDPHeader udpHeader;           // UDP header
    std::vector<uint8_t> data;     // Payload (SIMBA data)
};

} // namespace pcap

#pragma pack(pop)

#endif // PCAP_MESSAGES_HPP
