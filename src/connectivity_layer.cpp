#include "../include/connectivity_layer.h"
#include <iostream>

ConnectivityLayer::ConnectivityLayer() {
    config_ = ConfigManager::getInstance();
}

ConnectivityLayer::~ConnectivityLayer() {
    // Disconnect from all markets
    for (auto& pair : connections_) {
        if (pair.second.connected) {
            disconnect(pair.first);
        }
    }
}

bool ConnectivityLayer::initialize() {
    return true;
}

bool ConnectivityLayer::connect(Market market, const std::string& endpoint, const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    
    auto& conn_info = connections_[market];
    conn_info.endpoint = endpoint;
    conn_info.username = username;
    conn_info.password = password;
    
    // In a real implementation, this would establish the actual connection
    // For now, we'll simulate a successful connection
    conn_info.connected = true;
    conn_info.last_heartbeat = std::chrono::steady_clock::now();
    
    std::cout << "Connected to market " << static_cast<int>(market) << " at " << endpoint << std::endl;
    return true;
}

bool ConnectivityLayer::disconnect(Market market) {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    
    auto it = connections_.find(market);
    if (it != connections_.end()) {
        it->second.connected = false;
        std::cout << "Disconnected from market " << static_cast<int>(market) << std::endl;
        return true;
    }
    
    return false;
}

bool ConnectivityLayer::sendOrder(const Order& order) {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    
    auto it = connections_.find(order.market);
    if (it != connections_.end() && it->second.connected) {
        // In a real implementation, this would send the order via FIX or other protocol
        // For now, we'll just log the action
        std::cout << "Sending order " << order.order_id << " to market " << static_cast<int>(order.market) << std::endl;
        
        messages_sent_++;
        return true;
    }
    
    return false;
}

bool ConnectivityLayer::cancelOrder(OrderId order_id, Market market) {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    
    auto it = connections_.find(market);
    if (it != connections_.end() && it->second.connected) {
        // In a real implementation, this would send a cancel order via FIX or other protocol
        // For now, we'll just log the action
        std::cout << "Sending cancel request for order " << order_id << " to market " << static_cast<int>(market) << std::endl;
        
        messages_sent_++;
        return true;
    }
    
    return false;
}

bool ConnectivityLayer::subscribeToMarketData(Market market, const std::vector<InstrumentId>& instruments) {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    
    auto it = connections_.find(market);
    if (it != connections_.end() && it->second.connected) {
        // In a real implementation, this would subscribe to market data
        // For now, we'll just log the action
        std::cout << "Subscribed to market data for " << instruments.size() << " instruments on market " << static_cast<int>(market) << std::endl;
        
        messages_sent_++;
        return true;
    }
    
    return false;
}

bool ConnectivityLayer::isConnected(Market market) const {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    
    auto it = connections_.find(market);
    if (it != connections_.end()) {
        return it->second.connected;
    }
    
    return false;
}