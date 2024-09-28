#ifndef PACKET_DECODER_HPP
#define PACKET_DECODER_HPP

#include <vector>
#include <string>
#include <span>

class PacketDecoder
{
public:
    // Pure virtual function for decoding the packet data
    virtual void decode(std::span<const uint8_t> data) = 0;

    // Pure virtual function for converting decoded data to JSON format
    virtual std::string toJSON() const = 0;

    // Virtual destructor to ensure proper cleanup of derived classes
    virtual ~PacketDecoder() = default;
};

#endif // PACKET_DECODER_HPP
