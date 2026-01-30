#ifndef EXECUTION_MANAGEMENT_SYSTEM_H
#define EXECUTION_MANAGEMENT_SYSTEM_H

#include <memory>
#include <functional>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include "common_types.h"
#include "config.h"

class ExecutionManagementSystem {
public:
    using ExecutionCallback = std::function<void(const Order&)>;

    ExecutionManagementSystem();
    ~ExecutionManagementSystem();

    // Initialize the EMS
    bool initialize(ExecutionCallback callback);

    // Send order to market
    bool sendOrderToMarket(const Order& order);

    // Cancel order at market
    bool sendCancelToMarket(OrderId order_id);

    // Modify order at market
    bool sendModifyToMarket(OrderId order_id, const Order& new_order);

    // Connect to market
    bool connectToMarket(Market market, const std::string& endpoint);

    // Get statistics
    uint64_t getOrdersSent() const { return orders_sent_; }
    uint64_t getOrdersAcked() const { return orders_acked_; }

    // Start and stop methods
    void start();
    void stop();

private:
    // Worker thread for sending orders
    void sendWorker();

    // Worker thread for receiving executions
    void receiveWorker();

    // Queue for outgoing orders
    struct OutgoingOrderQueue {
        std::queue<Order> queue_;
        std::mutex mutex_;
        std::atomic<bool> stopped_{false};
    };

    // Queue for incoming executions
    struct IncomingExecutionQueue {
        std::queue<Order> queue_;
        std::mutex mutex_;
        std::atomic<bool> stopped_{false};
    };

    std::unique_ptr<OutgoingOrderQueue> outgoing_queue_;
    std::unique_ptr<IncomingExecutionQueue> incoming_queue_;

    std::unique_ptr<std::thread> send_thread_;
    std::unique_ptr<std::thread> receive_thread_;

    std::atomic<bool> running_{false};
    std::atomic<uint64_t> orders_sent_{0};
    std::atomic<uint64_t> orders_acked_{0};

    // Callback for execution updates
    ExecutionCallback execution_callback_;

    // Market connections
    std::unordered_map<Market, std::string> market_endpoints_;
};

#endif // EXECUTION_MANAGEMENT_SYSTEM_H