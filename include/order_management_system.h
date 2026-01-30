#ifndef ORDER_MANAGEMENT_SYSTEM_H
#define ORDER_MANAGEMENT_SYSTEM_H

#include <memory>
#include <unordered_map>
#include <mutex>
#include <queue>
#include <functional>
#include <atomic>
#include "common_types.h"
#include "config.h"

class OrderManagementSystem {
public:
    using OrderCallback = std::function<void(const Order&)>;

    OrderManagementSystem();
    ~OrderManagementSystem();

    // Initialize the OMS
    bool initialize(OrderCallback callback);

    // Submit a new order
    OrderId submitOrder(const Order& order);

    // Cancel an existing order
    bool cancelOrder(OrderId order_id);

    // Modify an existing order
    bool modifyOrder(OrderId order_id, const Order& new_order);

    // Get order status
    OrderState getOrderStatus(OrderId order_id) const;

    // Get order by ID
    Order getOrder(OrderId order_id) const;

    // Get statistics
    uint64_t getOrdersSubmitted() const { return orders_submitted_; }
    uint64_t getOrdersFilled() const { return orders_filled_; }

private:
    // Validate order before submission
    bool validateOrder(const Order& order);

    // Update order state
    void updateOrderState(OrderId order_id, OrderState new_state);

    // Thread-safe order storage
    mutable std::mutex orders_mutex_;
    std::unordered_map<OrderId, Order> orders_;

    // Atomic counters
    std::atomic<OrderId> next_order_id_{1};
    std::atomic<uint64_t> orders_submitted_{0};
    std::atomic<uint64_t> orders_filled_{0};

    // Callback for order updates
    OrderCallback order_callback_;

    // System configuration
    SystemConfig config_;
};

#endif // ORDER_MANAGEMENT_SYSTEM_H