// Author: Mert Özer
// Email: mertt.ozer@hotmail.com

#include "../include/pcap_parser.hpp"
#include <iostream>

int main(const int argc, const char* argv[])
{
    // Ensure correct number of arguments
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <pcap file path>"
                  << " <output file path>" << std::endl;
        return EXIT_FAILURE;
    }

    // Capture the file paths from the command line arguments
    const std::string pcapFileName = argv[1];
    const std::string outputFileName = argv[2];

    std::cout << "Decoding..." << std::endl;

    try {
        // Initialize the parser and start parsing
        pcap::PcapParser parser(pcapFileName, outputFileName);
        parser.parse();

        std::cout << "Decoded data has been successfully saved to "
                  << outputFileName << std::endl;

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
