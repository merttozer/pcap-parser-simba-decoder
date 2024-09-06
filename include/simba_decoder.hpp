#ifndef SIMBA_DECODER_HPP
#define SIMBA_DECODER_HPP

#include "simba_messages.hpp"
#include <vector>
#include <cstdint>
#include <string>

namespace simba {

class SimbaDecoder
{
public:
    // Constructor: Initialize decoder with packet data
    explicit SimbaDecoder(const std::vector<uint8_t>& packetData);

    // Main method to decode the packet data into structured messages
    void decode();

    // Convert the decoded messages to a JSON string
    std::string toJSON() const;

private:
    // Reference to the packet data to decode
    const std::vector<uint8_t>& packetData;

    // Parse headers
    MarketDataPacketHeader parseMarketDataPacketHeader(
      size_t& offset) const noexcept;
    IncrementalPacketHeader parseIncrementalPacketHeader(
      size_t& offset) const noexcept;
    SBEHeader parseSBEHeader(size_t& offset) const noexcept;

    // Decoders for each message type
    OrderUpdate decodeOrderUpdate(size_t& offset) const noexcept;
    OrderExecution decodeOrderExecution(size_t& offset) const noexcept;
    OrderBookSnapshot decodeOrderBookSnapshot(size_t& offset) const;

    // Storage for decoded messages
    std::vector<OrderUpdate> orderUpdates;
    std::vector<OrderExecution> orderExecutions;
    std::vector<OrderBookSnapshot> orderBookSnapshots;
};

} // namespace simba

#endif // SIMBA_DECODER_HPP
