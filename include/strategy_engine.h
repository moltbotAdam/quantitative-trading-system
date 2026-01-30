#ifndef STRATEGY_ENGINE_H
#define STRATEGY_ENGINE_H

#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include "common_types.h"
#include "config.h"

// Base strategy interface
class Strategy {
public:
    virtual ~Strategy() = default;
    
    virtual void onTick(const Tick& tick) = 0;
    virtual void onOrderUpdate(const Order& order) = 0;
    virtual std::vector<Order> generateSignals() = 0;
    virtual std::string getName() const = 0;
    virtual bool isActive() const = 0;
};

class StrategyEngine {
public:
    using SignalCallback = std::function<void(const Order&)>;

    StrategyEngine();
    ~StrategyEngine();

    // Initialize the strategy engine
    bool initialize(SignalCallback callback);

    // Register a new strategy
    bool registerStrategy(std::unique_ptr<Strategy> strategy);

    // Start all strategies
    void start();

    // Stop all strategies
    void stop();

    // Process incoming market data
    void processTick(const Tick& tick);

    // Process order updates
    void processOrderUpdate(const Order& order);

    // Get active strategies count
    size_t getActiveStrategiesCount() const;

    // Get strategy by name
    Strategy* getStrategyByName(const std::string& name);

private:
    // Vector to hold registered strategies
    std::vector<std::unique_ptr<Strategy>> strategies_;

    // Thread safety
    mutable std::mutex strategies_mutex_;

    // Callback for generated signals
    SignalCallback signal_callback_;

    // Engine state
    std::atomic<bool> running_{false};
};

// Example concrete strategy class
class SimpleMeanReversionStrategy : public Strategy {
public:
    explicit SimpleMeanReversionStrategy(InstrumentId instrument_id, double threshold);
    
    void onTick(const Tick& tick) override;
    void onOrderUpdate(const Order& order) override;
    std::vector<Order> generateSignals() override;
    std::string getName() const override { return "SimpleMeanReversion"; }
    bool isActive() const override { return active_; }

private:
    InstrumentId instrument_id_;
    double threshold_;
    bool active_{true};
    
    // Simple moving average tracking
    std::vector<Price> prices_;
    size_t window_size_{100};
    
    // Last order tracking
    OrderId last_order_id_{0};
    OrderState last_order_state_{OrderState::NEW};
};

#endif // STRATEGY_ENGINE_H