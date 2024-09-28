#ifndef PCAP_PARSER_HPP
#define PCAP_PARSER_HPP

#include "pcap_messages.hpp"
#include "packet_decoder.hpp"
#include <fstream>
#include <memory>

namespace pcap {

// Class to parse pcap files
class PcapParser
{
public:
    explicit PcapParser(const std::string& filename,
                        const std::string& outputFile,
                        std::unique_ptr<PacketDecoder> decoder);
    void parse();

private:
    std::string filename;
    std::string outputFile;
    PcapGlobalHeader globalHeader;
    std::unique_ptr<PacketDecoder> decoder; // Decoder object for packet data

    // Methods to parse different parts of the packet
    static PcapGlobalHeader parseGlobalHeader(std::ifstream& file);
    static PcapPacket parsePacket(std::ifstream& file);
    static EthernetHeader parseEthernetHeader(std::ifstream& file);
    static IPv4Header parseIPv4Header(std::ifstream& file);
    static UDPHeader parseUDPHeader(std::ifstream& file);

    // Methods to process and display packet information
    void saveDecodedPacket(const PcapPacket& packet, std::ofstream& outFile);
};

} // namespace pcap

#endif // PCAP_PARSER_HPP
