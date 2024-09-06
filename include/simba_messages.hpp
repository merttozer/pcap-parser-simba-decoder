#ifndef SIMBA_MESSAGES_HPP
#define SIMBA_MESSAGES_HPP

#include <cstdint>
#include <vector>
#include <cstddef> // Include this header for size_t
#include <type_traits>

#pragma pack(push, 1) // Ensures structures are packed with no padding

namespace simba {

// Structure for decimal values with a fixed exponent
struct Decimal5
{
    int64_t mantissa;
    static constexpr double exponent = 1e-5;
};

// Structure for nullable decimal values
struct Decimal5NULL
{
    static constexpr int64_t MAX_VALUE = 9223372036854775806;
    static constexpr int64_t NULL_VALUE = 9223372036854775807;

    int64_t mantissa;
    static constexpr double exponent = 1e-5;
};

// Market Data Packet Header structure
struct MarketDataPacketHeader
{
    bool IsIncremental() const noexcept { return msg_flags & 0x8; }

    uint32_t msg_seq_num;
    uint16_t msg_size;
    uint16_t msg_flags;
    uint64_t sending_time;

    static constexpr size_t SIZE = sizeof(msg_seq_num) + sizeof(msg_size) +
                                   sizeof(msg_flags) + sizeof(sending_time);
};
static_assert(MarketDataPacketHeader::SIZE == 16,
              "MarketDataPacketHeader size is incorrect");

// Incremental Packet Header structure
struct IncrementalPacketHeader
{
    static constexpr uint32_t EXCHANGE_TRADING_SESSION_ID_NULL = 4294967295;

    uint64_t transact_time;
    uint32_t exchange_trading_session_id;

    static constexpr size_t SIZE =
      sizeof(transact_time) + sizeof(exchange_trading_session_id);
};
static_assert(IncrementalPacketHeader::SIZE == 12,
              "IncrementalPacketHeader size is incorrect");

// Structure for SBE Header
struct SBEHeader
{
    uint16_t block_length;
    uint16_t template_id;
    uint16_t schema_id;
    uint16_t version;

    static constexpr size_t SIZE = sizeof(block_length) + sizeof(template_id) +
                                   sizeof(schema_id) + sizeof(version);
};
static_assert(SBEHeader::SIZE == 8, "SBEHeader size is incorrect");

// Enumeration for MDFlagsSet
enum class MDFlagsSet : uint64_t
{
    Day = 0x1,
    IOC = 0x2,
    NonQuote = 0x4,
    EndOfTransaction = 0x1000,
    SecondLeg = 0x4000,
    FOK = 0x80000,
    Replace = 0x100000,
    Cancel = 0x200000,
    MassCancel = 0x400000,
    Negotiated = 0x4000000,
    MultiLeg = 0x8000000,
    CrossTrade = 0x20000000,
    COD = 0x100000000,
    ActiveSide = 0x20000000000,
    PassiveSide = 0x40000000000,
    Synthetic = 0x200000000000,
    RFS = 0x400000000000,
    SyntheticPassive = 0x200000000000000,
};

// Enumeration for MDUpdateAction
enum class MDUpdateAction : uint8_t
{
    New = 0,
    Change = 1,
    Delete = 2,
};

// Enumeration for MDEntryType
enum class MDEntryType : char
{
    Bid = '0',
    Offer = '1',
    EmptyBook = 'J',
};

// GroupSize structure
struct GroupSize
{
    uint16_t block_length;
    uint8_t num_in_group;

    static constexpr size_t SIZE = sizeof(block_length) + sizeof(num_in_group);
};
static_assert(GroupSize::SIZE == 3, "GroupSize size is incorrect");

// Structure for OrderUpdate message
struct OrderUpdate
{
    static constexpr uint16_t TEMPLATE_ID = 15;

    int64_t md_entry_id;
    Decimal5 md_entry_px;
    int64_t md_entry_size;
    MDFlagsSet md_flags;
    uint64_t md_flags2;
    int32_t security_id;
    uint32_t rpt_seq;
    MDUpdateAction md_update_action;
    MDEntryType md_entry_type;

    static constexpr size_t SIZE =
      sizeof(md_entry_id) + sizeof(md_entry_px.mantissa) +
      sizeof(md_entry_size) + sizeof(md_flags) + sizeof(md_flags2) +
      sizeof(security_id) + sizeof(rpt_seq) + sizeof(md_update_action) +
      sizeof(md_entry_type);
};
static_assert(OrderUpdate::SIZE == 50, "OrderUpdate size is incorrect");

// Structure for OrderExecution message
struct OrderExecution
{
    static constexpr uint16_t TEMPLATE_ID = 16;

    int64_t md_entry_id;
    Decimal5NULL md_entry_px;
    int64_t md_entry_size;
    Decimal5 last_px;
    int64_t last_qty;
    int64_t trade_id;
    MDFlagsSet md_flags;
    uint64_t md_flags2;
    int32_t security_id;
    uint32_t rpt_seq;
    MDUpdateAction md_update_action;
    MDEntryType md_entry_type;

    static constexpr size_t SIZE =
      sizeof(md_entry_id) + sizeof(md_entry_px.mantissa) +
      sizeof(md_entry_size) + sizeof(last_px.mantissa) + sizeof(last_qty) +
      sizeof(trade_id) + sizeof(md_flags) + sizeof(md_flags2) +
      sizeof(security_id) + sizeof(rpt_seq) + sizeof(md_update_action) +
      sizeof(md_entry_type);
};
static_assert(OrderExecution::SIZE == 74, "OrderExecution size is incorrect");

// Structure for OrderBookSnapshot message
struct OrderBookSnapshot
{
    static constexpr uint16_t TEMPLATE_ID = 17;

    int32_t security_id;
    uint32_t last_msg_seq_num_processed;
    uint32_t rpt_seq;
    uint32_t exchange_trading_session_id;
    GroupSize no_md_entries;

    struct Entry
    {
        int64_t md_entry_id;
        uint64_t transact_time;
        Decimal5NULL md_entry_px;
        int64_t md_entry_size;
        int64_t trade_id;
        MDFlagsSet md_flags;
        uint64_t md_flags2;
        MDEntryType md_entry_type;

        static constexpr size_t SIZE =
          sizeof(md_entry_id) + sizeof(transact_time) +
          sizeof(md_entry_px.mantissa) + sizeof(md_entry_size) +
          sizeof(trade_id) + sizeof(md_flags) + sizeof(md_flags2) +
          sizeof(md_entry_type);
    };

    std::vector<Entry> entries;

    static constexpr size_t SIZE =
      sizeof(security_id) + sizeof(last_msg_seq_num_processed) +
      sizeof(rpt_seq) + sizeof(exchange_trading_session_id) + GroupSize::SIZE;
};
static_assert(OrderBookSnapshot::SIZE == 19,
              "OrderBookSnapshot size is incorrect");

} // namespace simba

#pragma pack(pop) // Restore original packing

#endif // SIMBA_MESSAGES_HPP
