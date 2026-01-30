#include "../include/strategy_engine.h"
#include <iostream>
#include <numeric>

StrategyEngine::StrategyEngine() = default;

StrategyEngine::~StrategyEngine() {
    stop();
}

bool StrategyEngine::initialize(SignalCallback callback) {
    if (!callback) {
        return false;
    }
    
    signal_callback_ = callback;
    return true;
}

bool StrategyEngine::registerStrategy(std::unique_ptr<Strategy> strategy) {
    if (!strategy) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(strategies_mutex_);
    strategies_.push_back(std::move(strategy));
    return true;
}

void StrategyEngine::start() {
    running_ = true;
}

void StrategyEngine::stop() {
    running_ = false;
}

void StrategyEngine::processTick(const Tick& tick) {
    if (!running_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(strategies_mutex_);
    
    for (auto& strategy : strategies_) {
        if (strategy->isActive()) {
            strategy->onTick(tick);
            
            // Check for any generated signals
            auto signals = strategy->generateSignals();
            for (const auto& signal : signals) {
                if (signal_callback_) {
                    signal_callback_(signal);
                }
            }
        }
    }
}

void StrategyEngine::processOrderUpdate(const Order& order) {
    if (!running_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(strategies_mutex_);
    
    for (auto& strategy : strategies_) {
        if (strategy->isActive()) {
            strategy->onOrderUpdate(order);
        }
    }
}

size_t StrategyEngine::getActiveStrategiesCount() const {
    std::lock_guard<std::mutex> lock(strategies_mutex_);
    
    size_t count = 0;
    for (const auto& strategy : strategies_) {
        if (strategy->isActive()) {
            count++;
        }
    }
    
    return count;
}

Strategy* StrategyEngine::getStrategyByName(const std::string& name) {
    std::lock_guard<std::mutex> lock(strategies_mutex_);
    
    for (auto& strategy : strategies_) {
        if (strategy->getName() == name) {
            return strategy.get();
        }
    }
    
    return nullptr;
}

// Implementation for SimpleMeanReversionStrategy
SimpleMeanReversionStrategy::SimpleMeanReversionStrategy(InstrumentId instrument_id, double threshold)
    : instrument_id_(instrument_id), threshold_(threshold) {}

void SimpleMeanReversionStrategy::onTick(const Tick& tick) {
    if (tick.instrument_id != instrument_id_) {
        return;
    }
    
    // Add the price to our price history
    Price mid_price = (tick.bid_price + tick.ask_price) / 2.0;
    prices_.push_back(mid_price);
    
    // Keep only the last N prices
    if (prices_.size() > window_size_) {
        prices_.erase(prices_.begin());
    }
}

void SimpleMeanReversionStrategy::onOrderUpdate(const Order& order) {
    if (order.instrument_id != instrument_id_) {
        return;
    }
    
    last_order_id_ = order.order_id;
    last_order_state_ = order.state;
}

std::vector<Order> SimpleMeanReversionStrategy::generateSignals() {
    std::vector<Order> signals;
    
    if (prices_.size() < 2) {
        return signals; // Not enough data yet
    }
    
    // Calculate simple moving average
    double sum = std::accumulate(prices_.begin(), prices_.end(), 0.0);
    double sma = sum / prices_.size();
    
    // Current price (using mid-price)
    Price current_price = prices_.back();
    
    // Check if price deviates significantly from SMA
    double deviation = (current_price - sma) / sma;
    
    // Only generate signals if deviation exceeds threshold and no active order
    if (std::abs(deviation) > threshold_ && 
        (last_order_state_ == OrderState::FILLED || last_order_state_ == OrderState::CANCELLED || last_order_id_ == 0)) {
        
        Order signal;
        signal.instrument_id = instrument_id_;
        signal.type = OrderType::MARKET;
        signal.quantity = 100; // Fixed quantity for simplicity
        signal.timestamp = std::chrono::high_resolution_clock::now();
        
        if (deviation > threshold_) {
            // Price is above SMA - sell (mean reversion)
            signal.side = OrderSide::SELL;
            signal.price = current_price;
        } else if (deviation < -threshold_) {
            // Price is below SMA - buy (mean reversion)
            signal.side = OrderSide::BUY;
            signal.price = current_price;
        }
        
        signals.push_back(signal);
    }
    
    return signals;
}