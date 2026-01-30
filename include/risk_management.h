#ifndef RISK_MANAGEMENT_H
#define RISK_MANAGEMENT_H

#include <memory>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <chrono>
#include <thread>
#include "common_types.h"
#include "config.h"

struct Position {
    InstrumentId instrument_id;
    double quantity;
    double average_price;
    double unrealized_pnl;
    double realized_pnl;
    std::chrono::time_point<std::chrono::system_clock> last_update;
};

class RiskManagement {
public:
    RiskManagement();
    ~RiskManagement();

    // Initialize risk management system
    bool initialize(const RiskLimits& limits);

    // Check if order passes risk checks
    bool checkOrder(const Order& order);

    // Update position after fill
    void updatePosition(const Order& order);

    // Check if position is within limits
    bool checkPosition(const Position& position);

    // Get current position for instrument
    Position getPosition(InstrumentId instrument_id) const;

    // Get total portfolio value
    double getTotalValue() const;

    // Reset daily statistics
    void resetDailyStats();

private:
    // Calculate value at risk
    double calculateVaR(InstrumentId instrument_id, double quantity);

    // Check various risk limits
    bool checkPositionSize(const Order& order);
    bool checkDailyLoss();
    bool checkOrderValue(const Order& order);
    bool checkDrawdown();
    bool checkRateOfOrders(const Order& order);

    // Thread-safe position storage
    mutable std::mutex positions_mutex_;
    std::unordered_map<InstrumentId, Position> positions_;

    // Daily statistics
    mutable std::mutex daily_stats_mutex_;
    double daily_pnl_{0.0};
    double peak_portfolio_value_{0.0};
    double current_drawdown_{0.0};
    int orders_last_second_{0};
    std::chrono::time_point<std::chrono::steady_clock> last_second_check_;

    // Risk limits
    RiskLimits risk_limits_;

    // Atomic flags
    std::atomic<bool> initialized_{false};
    std::atomic<double> total_portfolio_value_{0.0};
};

#endif // RISK_MANAGEMENT_H