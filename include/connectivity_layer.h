#ifndef CONNECTIVITY_LAYER_H
#define CONNECTIVITY_LAYER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <chrono>
#include "common_types.h"
#include "config.h"

class ConnectivityLayer {
public:
    ConnectivityLayer();
    ~ConnectivityLayer();

    // Initialize connectivity layer
    bool initialize();

    // Connect to market
    bool connect(Market market, const std::string& endpoint, const std::string& username, const std::string& password);

    // Disconnect from market
    bool disconnect(Market market);

    // Send order to market
    bool sendOrder(const Order& order);

    // Cancel order at market
    bool cancelOrder(OrderId order_id, Market market);

    // Subscribe to market data
    bool subscribeToMarketData(Market market, const std::vector<InstrumentId>& instruments);

    // Get connection status
    bool isConnected(Market market) const;

    // Get statistics
    uint64_t getMessagesSent() const { return messages_sent_; }
    uint64_t getMessagesReceived() const { return messages_received_; }

private:
    // Connection information structure
    struct ConnectionInfo {
        std::string endpoint;
        std::string username;
        std::string password;
        bool connected{false};
        std::chrono::time_point<std::chrono::steady_clock> last_heartbeat;
    };

    // Thread-safe connection map
    mutable std::mutex connections_mutex_;
    std::unordered_map<Market, ConnectionInfo> connections_;

    // Message counters
    std::atomic<uint64_t> messages_sent_{0};
    std::atomic<uint64_t> messages_received_{0};

    // System configuration
    SystemConfig config_;
};

#endif // CONNECTIVITY_LAYER_H