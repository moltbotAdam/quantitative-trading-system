#include "../include/market_data_handler.h"
#include <iostream>
#include <algorithm>

MarketDataHandler::MarketDataHandler() {
    tick_queue_ = std::make_unique<TickQueue>();
    subscribed_instruments_ = std::make_unique<SubscribedInstruments>();
}

MarketDataHandler::~MarketDataHandler() {
    stop();
    if (worker_thread_ && worker_thread_->joinable()) {
        worker_thread_->join();
    }
}

bool MarketDataHandler::initialize(TickCallback callback) {
    if (!callback) {
        return false;
    }
    
    tick_callback_ = callback;
    return true;
}

bool MarketDataHandler::connectToMarket(Market market, const std::string& endpoint) {
    // In a real implementation, this would establish a connection to the market data feed
    // For now, we'll just store the endpoint
    market_connections_[market] = endpoint;
    std::cout << "Connected to market " << static_cast<int>(market) << " at " << endpoint << std::endl;
    return true;
}

void MarketDataHandler::start() {
    if (running_.exchange(true)) {
        return; // Already running
    }
    
    worker_thread_ = std::make_unique<std::thread>(&MarketDataHandler::workerThread, this);
}

void MarketDataHandler::stop() {
    if (!running_.exchange(false)) {
        return; // Not running
    }
    
    tick_queue_->stopped_ = true;
    tick_queue_->cv_.notify_all();
}

bool MarketDataHandler::subscribe(InstrumentId instrument_id) {
    std::lock_guard<std::mutex> lock(subscribed_instruments_->mutex_);
    return subscribed_instruments_->set_.insert(instrument_id).second;
}

bool MarketDataHandler::unsubscribe(InstrumentId instrument_id) {
    std::lock_guard<std::mutex> lock(subscribed_instruments_->mutex_);
    return subscribed_instruments_->set_.erase(instrument_id) > 0;
}

void MarketDataHandler::workerThread() {
    while (running_) {
        std::unique_lock<std::mutex> lock(tick_queue_->mutex_);
        
        // Wait for ticks or timeout
        if (tick_queue_->cv_.wait_for(lock, std::chrono::milliseconds(10), 
                                     [this] { return !tick_queue_->queue_.empty() || tick_queue_->stopped_; })) {
            
            if (tick_queue_->stopped_) {
                break;
            }
            
            // Process all available ticks
            while (!tick_queue_->queue_.empty()) {
                Tick tick = tick_queue_->queue_.front();
                tick_queue_->queue_.pop();
                lock.unlock();
                
                processTick(tick);
                
                lock.lock();
            }
        }
    }
}

void MarketDataHandler::addTick(const Tick& tick) {
    // Add tick to the queue for processing
    {
        std::lock_guard<std::mutex> lock(tick_queue_->mutex_);
        tick_queue_->queue_.push(tick);
    }
    
    // Notify the worker thread
    tick_queue_->cv_.notify_one();
}

void MarketDataHandler::processTick(const Tick& tick) {
    // Only process if instrument is subscribed
    {
        std::lock_guard<std::mutex> lock(subscribed_instruments_->mutex_);
        if (subscribed_instruments_->set_.find(tick.instrument_id) == subscribed_instruments_->set_.end()) {
            return;
        }
    }
    
    // Update tick counter
    ticks_received_++;
    
    // Call the registered callback
    if (tick_callback_) {
        tick_callback_(tick);
    }
}