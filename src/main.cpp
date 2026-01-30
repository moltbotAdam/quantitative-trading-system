#include "../include/market_data_handler.h"
#include "../include/order_management_system.h"
#include "../include/execution_management_system.h"
#include "../include/risk_management.h"
#include "../include/strategy_engine.h"
#include "../include/connectivity_layer.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <memory>

int main() {
    std::cout << "Initializing High-Performance Trading System..." << std::endl;
    
    // Initialize components
    auto market_data_handler = std::make_unique<MarketDataHandler>();
    auto order_management = std::make_unique<OrderManagementSystem>();
    auto execution_management = std::make_unique<ExecutionManagementSystem>();
    auto risk_management = std::make_unique<RiskManagement>();
    auto strategy_engine = std::make_unique<StrategyEngine>();
    auto connectivity_layer = std::make_unique<ConnectivityLayer>();
    
    // Initialize risk limits
    RiskLimits limits;
    limits.max_position_size = 10000;
    limits.max_daily_loss = 100000;
    limits.max_order_value = 50000;
    limits.max_drawdown = 50000;
    limits.max_orders_per_second = 100;
    
    // Initialize risk management
    risk_management->initialize(limits);
    
    // Set up callbacks
    auto tick_callback = [&](const Tick& tick) {
        // Process tick through strategy engine
        strategy_engine->processTick(tick);
    };
    
    auto order_callback = [&](const Order& order) {
        // Process order updates through risk management
        if (order.state == OrderState::FILLED || order.state == OrderState::PARTIALLY_FILLED) {
            risk_management->updatePosition(order);
        }
        
        // Forward to execution management
        execution_management->sendOrderToMarket(order);
    };
    
    auto signal_callback = [&](const Order& order) {
        // Process strategy signals through risk management
        if (risk_management->checkOrder(order)) {
            // Submit to order management system
            OrderId id = order_management->submitOrder(order);
            std::cout << "Strategy signal processed, order ID: " << id << std::endl;
        } else {
            std::cout << "Risk check failed for strategy signal" << std::endl;
        }
    };
    
    // Initialize components with callbacks
    market_data_handler->initialize(tick_callback);
    order_management->initialize(order_callback);
    execution_management->initialize(order_callback);
    strategy_engine->initialize(signal_callback);
    connectivity_layer->initialize();
    
    // Connect to markets
    market_data_handler->connectToMarket(Market::USA_NYSE, "nyse_endpoint");
    market_data_handler->connectToMarket(Market::CHINA_SSE, "sse_endpoint");
    market_data_handler->connectToMarket(Market::HONG_KONG, "hkex_endpoint");
    
    connectivity_layer->connect(Market::USA_NYSE, "nyse_fix_endpoint", "user", "pass");
    connectivity_layer->connect(Market::CHINA_SSE, "sse_fix_endpoint", "user", "pass");
    connectivity_layer->connect(Market::HONG_KONG, "hkex_fix_endpoint", "user", "pass");
    
    // Register a sample strategy
    auto strategy = std::make_unique<SimpleMeanReversionStrategy>(1, 0.02); // 2% threshold
    strategy_engine->registerStrategy(std::move(strategy));
    
    // Subscribe to some instruments
    market_data_handler->subscribe(1);
    market_data_handler->subscribe(2);
    
    // Start the market data handler
    market_data_handler->start();
    strategy_engine->start();
    
    std::cout << "Trading system initialized and running..." << std::endl;
    
    // Simulate some trading activity
    for (int i = 0; i < 10; ++i) {
        // Create a mock tick
        Tick tick;
        tick.instrument_id = 1;
        tick.bid_price = 100.0 + i * 0.1;
        tick.ask_price = 100.1 + i * 0.1;
        tick.bid_size = 1000;
        tick.ask_size = 1000;
        tick.timestamp = std::chrono::high_resolution_clock::now();
        
        // Add the tick to processing queue
        market_data_handler->addTick(tick);
        
        // Submit a test order
        Order order;
        order.instrument_id = 1;
        order.type = OrderType::LIMIT;
        order.side = OrderSide::BUY;
        order.price = 100.0;
        order.quantity = 100;
        order.timestamp = std::chrono::high_resolution_clock::now();
        order.market = Market::USA_NYSE;
        
        OrderId id = order_management->submitOrder(order);
        std::cout << "Submitted test order, ID: " << id << std::endl;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Stop the system
    market_data_handler->stop();
    strategy_engine->stop();
    
    std::cout << "Trading system statistics:" << std::endl;
    std::cout << "Ticks received: " << market_data_handler->getTicksReceived() << std::endl;
    std::cout << "Orders submitted: " << order_management->getOrdersSubmitted() << std::endl;
    std::cout << "Orders filled: " << order_management->getOrdersFilled() << std::endl;
    std::cout << "Orders sent: " << execution_management->getOrdersSent() << std::endl;
    std::cout << "Orders acked: " << execution_management->getOrdersAcked() << std::endl;
    std::cout << "Messages sent: " << connectivity_layer->getMessagesSent() << std::endl;
    std::cout << "Messages received: " << connectivity_layer->getMessagesReceived() << std::endl;
    
    std::cout << "Trading system shutdown complete." << std::endl;
    
    return 0;
}