#include "../include/execution_management_system.h"
#include <iostream>

ExecutionManagementSystem::ExecutionManagementSystem() {
    outgoing_queue_ = std::make_unique<OutgoingOrderQueue>();
    incoming_queue_ = std::make_unique<IncomingExecutionQueue>();
}

ExecutionManagementSystem::~ExecutionManagementSystem() {
    stop();
    
    if (send_thread_ && send_thread_->joinable()) {
        send_thread_->join();
    }
    
    if (receive_thread_ && receive_thread_->joinable()) {
        receive_thread_->join();
    }
}

bool ExecutionManagementSystem::initialize(ExecutionCallback callback) {
    if (!callback) {
        return false;
    }
    
    execution_callback_ = callback;
    return true;
}

bool ExecutionManagementSystem::sendOrderToMarket(const Order& order) {
    if (!running_) {
        return false;
    }
    
    // Add order to outgoing queue
    {
        std::lock_guard<std::mutex> lock(outgoing_queue_->mutex_);
        outgoing_queue_->queue_.push(order);
    }
    
    orders_sent_++;
    return true;
}

bool ExecutionManagementSystem::sendCancelToMarket(OrderId order_id) {
    if (!running_) {
        return false;
    }
    
    // In a real implementation, this would create a cancel order
    // For now, we'll just log the action
    std::cout << "Cancellation request for order ID: " << order_id << std::endl;
    return true;
}

bool ExecutionManagementSystem::sendModifyToMarket(OrderId order_id, const Order& new_order) {
    if (!running_) {
        return false;
    }
    
    // In a real implementation, this would create a modify order
    // For now, we'll just log the action
    std::cout << "Modification request for order ID: " << order_id << std::endl;
    return true;
}

bool ExecutionManagementSystem::connectToMarket(Market market, const std::string& endpoint) {
    market_endpoints_[market] = endpoint;
    std::cout << "Connected to market " << static_cast<int>(market) << " at " << endpoint << std::endl;
    return true;
}

void ExecutionManagementSystem::start() {
    if (running_.exchange(true)) {
        return; // Already running
    }
    
    send_thread_ = std::make_unique<std::thread>(&ExecutionManagementSystem::sendWorker, this);
    receive_thread_ = std::make_unique<std::thread>(&ExecutionManagementSystem::receiveWorker, this);
}

void ExecutionManagementSystem::stop() {
    if (!running_.exchange(false)) {
        return; // Not running
    }
    
    outgoing_queue_->stopped_ = true;
    incoming_queue_->stopped_ = true;
}

void ExecutionManagementSystem::sendWorker() {
    while (running_) {
        // Check for orders to send
        {
            std::lock_guard<std::mutex> lock(outgoing_queue_->mutex_);
            
            if (!outgoing_queue_->queue_.empty()) {
                Order order = outgoing_queue_->queue_.front();
                outgoing_queue_->queue_.pop();
                
                // In a real implementation, this would send the order to the market
                // For now, we'll just simulate successful sending
                std::cout << "Sent order " << order.order_id << " to market " << static_cast<int>(order.market) << std::endl;
                
                // Simulate acknowledgment
                order.state = OrderState::NEW;
                if (execution_callback_) {
                    execution_callback_(order);
                }
                
                orders_acked_++;
            }
        }
        
        // Small delay to prevent busy-waiting
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
}

void ExecutionManagementSystem::receiveWorker() {
    while (running_) {
        // Check for incoming executions
        {
            std::lock_guard<std::mutex> lock(incoming_queue_->mutex_);
            
            if (!incoming_queue_->queue_.empty()) {
                Order execution = incoming_queue_->queue_.front();
                incoming_queue_->queue_.pop();
                
                // Process the execution
                if (execution_callback_) {
                    execution_callback_(execution);
                }
            }
        }
        
        // Small delay to prevent busy-waiting
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
}