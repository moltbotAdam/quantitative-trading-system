#include "../include/order_management_system.h"
#include <iostream>
#include <chrono>

OrderManagementSystem::OrderManagementSystem() {
    config_ = ConfigManager::getInstance();
}

OrderManagementSystem::~OrderManagementSystem() {
    // Clean up resources
}

bool OrderManagementSystem::initialize(OrderCallback callback) {
    if (!callback) {
        return false;
    }
    
    order_callback_ = callback;
    return true;
}

OrderId OrderManagementSystem::submitOrder(const Order& order) {
    if (!validateOrder(order)) {
        return 0; // Invalid order
    }
    
    OrderId new_id = next_order_id_.fetch_add(1);
    
    // Create a copy of the order with the new ID
    Order new_order = order;
    new_order.order_id = new_id;
    new_order.state = OrderState::PENDING_NEW;
    new_order.timestamp = std::chrono::high_resolution_clock::now();
    
    // Store the order
    {
        std::lock_guard<std::mutex> lock(orders_mutex_);
        orders_[new_id] = new_order;
    }
    
    // Update counter
    orders_submitted_++;
    
    // Call the callback
    if (order_callback_) {
        order_callback_(new_order);
    }
    
    return new_id;
}

bool OrderManagementSystem::cancelOrder(OrderId order_id) {
    std::lock_guard<std::mutex> lock(orders_mutex_);
    
    auto it = orders_.find(order_id);
    if (it == orders_.end()) {
        return false; // Order doesn't exist
    }
    
    // Only allow cancellation if order is not already filled or cancelled
    if (it->second.state == OrderState::FILLED || 
        it->second.state == OrderState::CANCELLED || 
        it->second.state == OrderState::REJECTED) {
        return false;
    }
    
    // Update state
    it->second.state = OrderState::PENDING_CANCEL;
    it->second.timestamp = std::chrono::high_resolution_clock::now();
    
    // Call the callback
    if (order_callback_) {
        order_callback_(it->second);
    }
    
    return true;
}

bool OrderManagementSystem::modifyOrder(OrderId order_id, const Order& new_order) {
    std::lock_guard<std::mutex> lock(orders_mutex_);
    
    auto it = orders_.find(order_id);
    if (it == orders_.end()) {
        return false; // Order doesn't exist
    }
    
    // Only allow modification if order is not filled or cancelled
    if (it->second.state == OrderState::FILLED || 
        it->second.state == OrderState::CANCELLED || 
        it->second.state == OrderState::REJECTED) {
        return false;
    }
    
    // Update order details
    it->second.type = new_order.type;
    it->second.price = new_order.price;
    it->second.quantity = new_order.quantity;
    it->second.timestamp = std::chrono::high_resolution_clock::now();
    
    // Call the callback
    if (order_callback_) {
        order_callback_(it->second);
    }
    
    return true;
}

OrderState OrderManagementSystem::getOrderStatus(OrderId order_id) const {
    std::lock_guard<std::mutex> lock(orders_mutex_);
    
    auto it = orders_.find(order_id);
    if (it != orders_.end()) {
        return it->second.state;
    }
    
    return OrderState::REJECTED; // Order doesn't exist
}

Order OrderManagementSystem::getOrder(OrderId order_id) const {
    std::lock_guard<std::mutex> lock(orders_mutex_);
    
    auto it = orders_.find(order_id);
    if (it != orders_.end()) {
        return it->second;
    }
    
    // Return invalid order if not found
    Order invalid_order{};
    invalid_order.order_id = 0;
    invalid_order.instrument_id = 0;
    invalid_order.type = OrderType::LIMIT;
    invalid_order.side = OrderSide::BUY;
    invalid_order.price = 0.0;
    invalid_order.quantity = 0.0;
    invalid_order.filled_quantity = 0.0;
    invalid_order.state = OrderState::REJECTED;
    invalid_order.market = Market::UNKNOWN;
    return invalid_order; // Default constructed invalid order
}

bool OrderManagementSystem::validateOrder(const Order& order) {
    // Basic validation checks
    if (order.instrument_id == 0) {
        return false;
    }
    
    if (order.quantity <= 0 || order.price <= 0) {
        return false;
    }
    
    // Additional validations can be added here
    return true;
}

void OrderManagementSystem::updateOrderState(OrderId order_id, OrderState new_state) {
    std::lock_guard<std::mutex> lock(orders_mutex_);
    
    auto it = orders_.find(order_id);
    if (it != orders_.end()) {
        it->second.state = new_state;
        it->second.timestamp = std::chrono::high_resolution_clock::now();
        
        // Update counters if order is filled
        if (new_state == OrderState::FILLED) {
            orders_filled_++;
        }
        
        // Call the callback
        if (order_callback_) {
            order_callback_(it->second);
        }
    }
}