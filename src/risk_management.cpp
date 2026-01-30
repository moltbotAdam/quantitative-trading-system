#include "../include/risk_management.h"
#include <iostream>
#include <cmath>
#include <algorithm>

RiskManagement::RiskManagement() {
    last_second_check_ = std::chrono::steady_clock::now();
}

RiskManagement::~RiskManagement() {
    // Clean up resources
}

bool RiskManagement::initialize(const RiskLimits& limits) {
    risk_limits_ = limits;
    initialized_ = true;
    return true;
}

bool RiskManagement::checkOrder(const Order& order) {
    if (!initialized_) {
        return false;
    }
    
    // Check all risk limits
    if (!checkPositionSize(order)) {
        std::cout << "Risk check failed: Position size limit exceeded" << std::endl;
        return false;
    }
    
    if (!checkOrderValue(order)) {
        std::cout << "Risk check failed: Order value limit exceeded" << std::endl;
        return false;
    }
    
    if (!checkRateOfOrders(order)) {
        std::cout << "Risk check failed: Too many orders per second" << std::endl;
        return false;
    }
    
    return true;
}

void RiskManagement::updatePosition(const Order& order) {
    if (order.state != OrderState::FILLED && order.state != OrderState::PARTIALLY_FILLED) {
        return; // Only update for filled orders
    }
    
    std::lock_guard<std::mutex> lock(positions_mutex_);
    
    auto& position = positions_[order.instrument_id];
    position.instrument_id = order.instrument_id;
    position.last_update = std::chrono::system_clock::now();
    
    // Update quantity based on order side
    if (order.side == OrderSide::BUY) {
        position.quantity += order.filled_quantity;
    } else {
        position.quantity -= order.filled_quantity;
    }
    
    // Update average price
    if (position.quantity != 0) {
        position.average_price = ((position.average_price * (position.quantity - order.filled_quantity)) + 
                                 (order.price * order.filled_quantity)) / position.quantity;
    }
}

Position RiskManagement::getPosition(InstrumentId instrument_id) const {
    std::lock_guard<std::mutex> lock(positions_mutex_);
    
    auto it = positions_.find(instrument_id);
    if (it != positions_.end()) {
        return it->second;
    }
    
    return {instrument_id, 0.0, 0.0, 0.0, 0.0, std::chrono::system_clock::now()};
}

double RiskManagement::getTotalValue() const {
    return total_portfolio_value_.load();
}

void RiskManagement::resetDailyStats() {
    std::lock_guard<std::mutex> lock(daily_stats_mutex_);
    daily_pnl_ = 0.0;
    current_drawdown_ = 0.0;
    orders_last_second_ = 0;
    last_second_check_ = std::chrono::steady_clock::now();
}

double RiskManagement::calculateVaR(InstrumentId instrument_id, double quantity) {
    // Simplified VaR calculation
    // In a real implementation, this would use historical volatility and correlations
    return quantity * 0.02; // Assuming 2% daily volatility
}

bool RiskManagement::checkPositionSize(const Order& order) {
    std::lock_guard<std::mutex> lock(positions_mutex_);
    
    auto it = positions_.find(order.instrument_id);
    double current_pos = (it != positions_.end()) ? it->second.quantity : 0.0;
    
    double new_quantity = (order.side == OrderSide::BUY) ? 
                         current_pos + order.quantity : 
                         current_pos - order.quantity;
    
    return std::abs(new_quantity) <= risk_limits_.max_position_size;
}

bool RiskManagement::checkDailyLoss() {
    std::lock_guard<std::mutex> lock(daily_stats_mutex_);
    return std::abs(daily_pnl_) <= risk_limits_.max_daily_loss;
}

bool RiskManagement::checkOrderValue(const Order& order) {
    double order_value = order.price * order.quantity;
    return order_value <= risk_limits_.max_order_value;
}

bool RiskManagement::checkDrawdown() {
    std::lock_guard<std::mutex> lock(daily_stats_mutex_);
    return std::abs(current_drawdown_) <= risk_limits_.max_drawdown;
}

bool RiskManagement::checkRateOfOrders(const Order& order) {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - last_second_check_).count();
    
    if (duration >= 1) {
        // Reset counter if more than a second has passed
        orders_last_second_ = 1;
        last_second_check_ = now;
    } else {
        orders_last_second_++;
    }
    
    return orders_last_second_ <= risk_limits_.max_orders_per_second;
}

bool RiskManagement::checkPosition(const Position& position) {
    return std::abs(position.quantity) <= risk_limits_.max_position_size;
}