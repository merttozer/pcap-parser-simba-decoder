#include "../include/simba_decoder.hpp"
#include <iostream>
#include <sstream>
#include <cstring>
#include <iomanip>

namespace simba {

// Constructor: Initialize the packet data reference
SimbaDecoder::SimbaDecoder(const std::vector<uint8_t>& packetData)
  : packetData(packetData)
{
}

// Main decode function that processes the entire packet data
void SimbaDecoder::decode()
{
    size_t offset = 0;

    // Step 1: Parse Market Data Packet Header
    MarketDataPacketHeader marketDataPacketHeader =
      parseMarketDataPacketHeader(offset);

    // Step 2: If it's an Incremental Packet, parse the Incremental Packet
    // Header
    if (marketDataPacketHeader.IsIncremental()) {
        IncrementalPacketHeader incrementalPacketHeader =
          parseIncrementalPacketHeader(offset);
    }

    // Step 3: Parse SBE Messages until the end of packet data
    while (offset < packetData.size()) {
        SBEHeader header = parseSBEHeader(offset);
        switch (header.template_id) {
            case OrderUpdate::TEMPLATE_ID:
                orderUpdates.emplace_back(decodeOrderUpdate(offset));
                break;
            case OrderExecution::TEMPLATE_ID:
                orderExecutions.emplace_back(decodeOrderExecution(offset));
                break;
            case OrderBookSnapshot::TEMPLATE_ID:
                orderBookSnapshots.emplace_back(
                  decodeOrderBookSnapshot(offset));
                break;
            default:
                // Skip unknown messages by advancing the offset by the block
                // length
                offset += header.block_length;
                break;
        }
    }
}

// Decode OrderUpdate message from the packet data
OrderUpdate SimbaDecoder::decodeOrderUpdate(size_t& offset) const noexcept
{
    OrderUpdate update;
    std::memcpy(&update, &packetData[offset], OrderUpdate::SIZE);
    offset += OrderUpdate::SIZE; // Advance the offset
    return update;
}

// Decode OrderExecution message from the packet data
OrderExecution SimbaDecoder::decodeOrderExecution(size_t& offset) const noexcept
{
    OrderExecution execution;
    std::memcpy(&execution, &packetData[offset], OrderExecution::SIZE);
    offset += OrderExecution::SIZE; // Advance the offset
    return execution;
}

// Decode OrderBookSnapshot message from the packet data
OrderBookSnapshot SimbaDecoder::decodeOrderBookSnapshot(size_t& offset) const
{
    OrderBookSnapshot snapshot{};

    // Copy the fixed-size portion of the snapshot structure
    std::memcpy(
      &snapshot.security_id, &packetData[offset], OrderBookSnapshot::SIZE);
    offset += OrderBookSnapshot::SIZE; // Advance the offset

    // Resize the vector to hold all entries
    snapshot.entries.resize(snapshot.no_md_entries.num_in_group);

    // Copy each entry individually
    for (auto& entry : snapshot.entries) {
        std::memcpy(
          &entry, &packetData[offset], OrderBookSnapshot::Entry::SIZE);
        offset += OrderBookSnapshot::Entry::SIZE; // Advance the offset
    }

    return snapshot;
}

// Parse the Market Data Packet Header
MarketDataPacketHeader SimbaDecoder::parseMarketDataPacketHeader(
  size_t& offset) const noexcept
{
    MarketDataPacketHeader header;
    std::memcpy(&header, &packetData[offset], MarketDataPacketHeader::SIZE);
    offset += MarketDataPacketHeader::SIZE; // Advance the offset
    return header;
}

// Parse the Incremental Packet Header
IncrementalPacketHeader SimbaDecoder::parseIncrementalPacketHeader(
  size_t& offset) const noexcept
{
    IncrementalPacketHeader header;
    std::memcpy(&header, &packetData[offset], IncrementalPacketHeader::SIZE);
    offset += IncrementalPacketHeader::SIZE; // Advance the offset
    return header;
}

// Parse the SBE (Simple Binary Encoding) Header
SBEHeader SimbaDecoder::parseSBEHeader(size_t& offset) const noexcept
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
