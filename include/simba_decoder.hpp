#ifndef SIMBA_DECODER_HPP
#define SIMBA_DECODER_HPP

#include "simba_messages.hpp"
#include "packet_decoder.hpp"
#include <vector>
#include <cstdint>
#include <string>
#include <span>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <future>

namespace simba {

class SimbaDecoder : public PacketDecoder
{
public:
    explicit SimbaDecoder();
    void decode(std::span<const uint8_t> packetData) override;
    std::string toJSON() const override;

private:
    std::vector<OrderUpdate> orderUpdates;
    std::vector<OrderExecution> orderExecutions;
    std::vector<OrderBookSnapshot> orderBookSnapshots;

    using DecodeFunction =
      std::function<void(std::span<const uint8_t>, size_t&)>;
    std::unordered_map<uint16_t, DecodeFunction> decodeMap;

    void initializeDecodeMap();

    static MarketDataPacketHeader parseMarketDataPacketHeader(
      std::span<const uint8_t> packetData,
      size_t& offset);
    static IncrementalPacketHeader parseIncrementalPacketHeader(
      std::span<const uint8_t> packetData,
      size_t& offset);
    static SBEHeader parseSBEHeader(std::span<const uint8_t> packetData,
                                    size_t& offset);

    void decodeOrderUpdate(std::span<const uint8_t> packetData, size_t& offset);
    void decodeOrderExecution(std::span<const uint8_t> packetData,
                              size_t& offset);
    void decodeOrderBookSnapshot(std::span<const uint8_t> packetData,
                                 size_t& offset);
};

} // namespace simba

#endif // SIMBA_DECODER_HPP
