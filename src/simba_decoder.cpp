#include "../include/simba_decoder.hpp"
#include <iostream>
#include <sstream>
#include <cstring>
#include <iomanip>

namespace simba {

// Constructor: Initialize the packet data reference
SimbaDecoder::SimbaDecoder()
{
    initializeDecodeMap();
}

void SimbaDecoder::initializeDecodeMap()
{
    decodeMap[OrderUpdate::TEMPLATE_ID] = [this](std::span<const uint8_t> data,
                                                 size_t& offset) {
        decodeOrderUpdate(data, offset);
    };
    decodeMap[OrderExecution::TEMPLATE_ID] =
      [this](std::span<const uint8_t> data, size_t& offset) {
          decodeOrderExecution(data, offset);
      };
    decodeMap[OrderBookSnapshot::TEMPLATE_ID] =
      [this](std::span<const uint8_t> data, size_t& offset) {
          decodeOrderBookSnapshot(data, offset);
      };
}

// Main decode function that processes the entire packet data
void SimbaDecoder::decode(std::span<const uint8_t> packetData)
{
    size_t offset = 0;
    MarketDataPacketHeader marketDataPacketHeader =
      parseMarketDataPacketHeader(packetData, offset);
    if (marketDataPacketHeader.IsIncremental()) {
        IncrementalPacketHeader incrementalPacketHeader =
          parseIncrementalPacketHeader(packetData, offset);
    }

    while (offset < packetData.size()) {
        SBEHeader header = parseSBEHeader(packetData, offset);
        auto it = decodeMap.find(header.template_id);
        if (it != decodeMap.end()) {
            it->second(packetData, offset);
        } else {
            // Skip unknown messages
            offset += header.block_length;
        }
    }
}

void SimbaDecoder::decodeOrderUpdate(std::span<const uint8_t> packetData,
                                     size_t& offset)
{
    OrderUpdate update;
    std::memcpy(&update, &packetData[offset], OrderUpdate::SIZE);
    offset += OrderUpdate::SIZE;
    orderUpdates.emplace_back(update);
}

void SimbaDecoder::decodeOrderExecution(std::span<const uint8_t> packetData,
                                        size_t& offset)
{
    OrderExecution execution;
    std::memcpy(&execution, &packetData[offset], OrderExecution::SIZE);
    offset += OrderExecution::SIZE;
    orderExecutions.emplace_back(execution);
}

void SimbaDecoder::decodeOrderBookSnapshot(std::span<const uint8_t> packetData,
                                           size_t& offset)
{
    OrderBookSnapshot snapshot;
    std::memcpy(&snapshot, &packetData[offset], OrderBookSnapshot::SIZE);
    offset += OrderBookSnapshot::SIZE;

    snapshot.entries.resize(snapshot.no_md_entries.num_in_group);
    for (auto& entry : snapshot.entries) {
        std::memcpy(
          &entry, &packetData[offset], OrderBookSnapshot::Entry::SIZE);
        offset += OrderBookSnapshot::Entry::SIZE;
    }
    orderBookSnapshots.emplace_back(snapshot);
}

// Parse the Market Data Packet Header
MarketDataPacketHeader SimbaDecoder::parseMarketDataPacketHeader(
  std::span<const uint8_t> packetData,
  size_t& offset)
{
    MarketDataPacketHeader header;
    std::memcpy(&header, &packetData[offset], MarketDataPacketHeader::SIZE);
    offset += MarketDataPacketHeader::SIZE; // Advance the offset
    return header;
}

// Parse the Incremental Packet Header
IncrementalPacketHeader SimbaDecoder::parseIncrementalPacketHeader(
  std::span<const uint8_t> packetData,
  size_t& offset)
{
    IncrementalPacketHeader header;
    std::memcpy(&header, &packetData[offset], IncrementalPacketHeader::SIZE);
    offset += IncrementalPacketHeader::SIZE; // Advance the offset
    return header;
}

// Parse the SBE (Simple Binary Encoding) Header
SBEHeader SimbaDecoder::parseSBEHeader(std::span<const uint8_t> packetData,
                                       size_t& offset)
{
    SBEHeader header;
    std::memcpy(&header, &packetData[offset], SBEHeader::SIZE);
    offset += SBEHeader::SIZE; // Advance the offset
    return header;
}

// Convert the decoded messages into a JSON string
std::string SimbaDecoder::toJSON() const
{
    std::ostringstream json;
    json << "{";

    // Serialize Order Updates
    json << "\"orderUpdates\":[";
    for (const auto& update : orderUpdates) {
        json << "{"
             << "\"md_entry_id\":" << update.md_entry_id << ","
             << "\"md_entry_px\":"
             << update.md_entry_px.mantissa * Decimal5::exponent << ","
             << "\"md_entry_size\":" << update.md_entry_size << ","
             << "\"md_flags\":" << static_cast<uint64_t>(update.md_flags) << ","
             << "\"md_flags2\":" << update.md_flags2 << ","
             << "\"security_id\":" << update.security_id << ","
             << "\"rpt_seq\":" << update.rpt_seq << ","
             << "\"md_update_action\":"
             << static_cast<int>(update.md_update_action) << ","
             << "\"md_entry_type\":\""
             << static_cast<char>(update.md_entry_type) << "\""
             << "},";
    }
    if (!orderUpdates.empty())
        json.seekp(-1, std::ios_base::end);
    json << "],";

    // Serialize Order Executions
    json << "\"orderExecutions\":[";
    for (const auto& execution : orderExecutions) {
        json << "{"
             << "\"md_entry_id\":" << execution.md_entry_id << ","
             << "\"md_entry_px\":"
             << (execution.md_entry_px.mantissa != Decimal5NULL::NULL_VALUE
                   ? execution.md_entry_px.mantissa * Decimal5NULL::exponent
                   : 0)
             << ","
             << "\"md_entry_size\":" << execution.md_entry_size << ","
             << "\"last_px\":"
             << execution.last_px.mantissa * Decimal5::exponent << ","
             << "\"last_qty\":" << execution.last_qty << ","
             << "\"trade_id\":" << execution.trade_id << ","
             << "\"md_flags\":" << static_cast<uint64_t>(execution.md_flags)
             << ","
             << "\"md_flags2\":" << execution.md_flags2 << ","
             << "\"security_id\":" << execution.security_id << ","
             << "\"rpt_seq\":" << execution.rpt_seq << ","
             << "\"md_update_action\":"
             << static_cast<int>(execution.md_update_action) << ","
             << "\"md_entry_type\":\""
             << static_cast<char>(execution.md_entry_type) << "\""
             << "},";
    }
    if (!orderExecutions.empty())
        json.seekp(-1, std::ios_base::end);
    json << "],";

    // Serialize Order Book Snapshots
    json << "\"orderBookSnapshots\":[";
    for (const auto& snapshot : orderBookSnapshots) {
        json << "{"
             << "\"security_id\":" << snapshot.security_id << ","
             << "\"last_msg_seq_num_processed\":"
             << snapshot.last_msg_seq_num_processed << ","
             << "\"rpt_seq\":" << snapshot.rpt_seq << ","
             << "\"exchange_trading_session_id\":"
             << snapshot.exchange_trading_session_id << ","
             << "\"no_md_entries\":{"
             << "\"block_length\":" << snapshot.no_md_entries.block_length
             << ","
             << "\"num_in_group\":"
             << static_cast<int>(snapshot.no_md_entries.num_in_group) << "},"
             << "\"entries\":[";
        for (const auto& entry : snapshot.entries) {
            json << "{"
                 << "\"md_entry_id\":" << entry.md_entry_id << ","
                 << "\"transact_time\":" << entry.transact_time << ","
                 << "\"md_entry_px\":"
                 << (entry.md_entry_px.mantissa != Decimal5NULL::NULL_VALUE
                       ? entry.md_entry_px.mantissa * Decimal5NULL::exponent
                       : 0)
                 << ","
                 << "\"md_entry_size\":" << entry.md_entry_size << ","
                 << "\"trade_id\":" << entry.trade_id << ","
                 << "\"md_flags\":" << static_cast<uint64_t>(entry.md_flags)
                 << ","
                 << "\"md_flags2\":" << entry.md_flags2 << ","
                 << "\"md_entry_type\":\""
                 << static_cast<uint64_t>(entry.md_entry_type) << "\""
                 << "},";
        }
        if (!snapshot.entries.empty())
            json.seekp(-1, std::ios_base::end);
        json << "]},";
    }
    if (!orderBookSnapshots.empty())
        json.seekp(-1, std::ios_base::end);
    json << "]";

    json << "}";

    return json.str();
}

} // namespace simba
