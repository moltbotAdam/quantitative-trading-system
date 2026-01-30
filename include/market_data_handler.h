#ifndef MARKET_DATA_HANDLER_H
#define MARKET_DATA_HANDLER_H

#include <memory>
#include <functional>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <unordered_set>
#include <unordered_map>
#include "common_types.h"
#include "config.h"

// Forward declaration
class TickProcessor;

class MarketDataHandler {
public:
    using TickCallback = std::function<void(const Tick&)>;

    MarketDataHandler();
    ~MarketDataHandler();

    // Initialize the handler with callback function
    bool initialize(TickCallback callback);

    // Connect to specific market
    bool connectToMarket(Market market, const std::string& endpoint);

    // Start receiving market data
    void start();

    // Stop receiving market data
    void stop();

    // Subscribe to specific instruments
    bool subscribe(InstrumentId instrument_id);

    // Unsubscribe from specific instruments
    bool unsubscribe(InstrumentId instrument_id);

    // Get statistics
    uint64_t getTicksReceived() const { return ticks_received_; }

    // Public method to add tick to processing queue
    void addTick(const Tick& tick);

private:
    // Internal worker thread function
    void workerThread();

    // Process incoming tick data (internal use)
    void processTick(const Tick& tick);

    // Queue for incoming ticks
    struct TickQueue {
        std::queue<Tick> queue_;
        std::mutex mutex_;
        std::condition_variable cv_;
        std::atomic<bool> stopped_{false};
    };

    std::unique_ptr<TickQueue> tick_queue_;
    std::unique_ptr<std::thread> worker_thread_;
    std::atomic<bool> running_{false};
    std::atomic<uint64_t> ticks_received_{0};

    // Callback for processed ticks
    TickCallback tick_callback_;

    // Market-specific connection handlers
    std::unordered_map<Market, std::string> market_connections_;
    
    // Subscribed instruments
    struct SubscribedInstruments {
        std::unordered_set<InstrumentId> set_;
        mutable std::mutex mutex_;
    };
    std::unique_ptr<SubscribedInstruments> subscribed_instruments_;
};

#endif // MARKET_DATA_HANDLER_H