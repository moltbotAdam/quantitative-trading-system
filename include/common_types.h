#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <string>
#include <cstdint>
#include <chrono>
#include <vector>

// Basic types
using OrderId = uint64_t;
using InstrumentId = uint64_t;
using Price = double;
using Quantity = double;

// Timestamp type for high-precision timing
using Timestamp = std::chrono::high_resolution_clock::time_point;

// Market identifiers
enum class Market : uint8_t {
    UNKNOWN = 0,
    CHINA_SSE = 1,    // Shanghai Stock Exchange
    CHINA_SZSE = 2,   // Shenzhen Stock Exchange
    HONG_KONG = 3,    // Hong Kong Exchange
    USA_NYSE = 4,     // New York Stock Exchange
    USA_NASDAQ = 5    // NASDAQ
};

// Order types
enum class OrderType : uint8_t {
    MARKET = 0,
    LIMIT = 1,
    STOP = 2,
    STOP_LIMIT = 3
};

// Order sides
enum class OrderSide : uint8_t {
    BUY = 0,
    SELL = 1
};

// Order states
enum class OrderState : uint8_t {
    PENDING_NEW = 0,
    NEW = 1,
    PARTIALLY_FILLED = 2,
    FILLED = 3,
    PENDING_CANCEL = 4,
    CANCELLED = 5,
    REJECTED = 6,
    EXPIRED = 7
};

// Market data structures
struct Tick {
    InstrumentId instrument_id;
    Price bid_price;
    Quantity bid_size;
    Price ask_price;
    Quantity ask_size;
    Timestamp timestamp;
};

struct Order {
    OrderId order_id;
    InstrumentId instrument_id;
    OrderType type;
    OrderSide side;
    Price price;
    Quantity quantity;
    Quantity filled_quantity;
    OrderState state;
    Market market;
    Timestamp timestamp;
};

// Risk limits structure
struct RiskLimits {
    double max_position_size;
    double max_daily_loss;
    double max_order_value;
    double max_drawdown;
    int max_orders_per_second;
};

#endif // COMMON_TYPES_H