// Author: Mert Özer
// Email: mertt.ozer@hotmail.com

#include "../include/pcap_parser.hpp"
#include "../include/simba_decoder.hpp"
#include <iostream>

int main(const int argc, const char* argv[])
{
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0]
                  << " <pcap file path> <output file path>" << std::endl;
        return EXIT_FAILURE;
    }

    const std::string pcapFileName = argv[1];
    const std::string outputFileName = argv[2];

    std::cout << "Decoding..." << std::endl;

    try {
        // Create a SimbaDecoder and pass it to the PcapParser
        std::unique_ptr<PacketDecoder> decoder =
          std::make_unique<simba::SimbaDecoder>();
        pcap::PcapParser parser(
          pcapFileName, outputFileName, std::move(decoder));
        parser.parse();

        std::cout << "Decoded data has been successfully saved to "
                  << outputFileName << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
