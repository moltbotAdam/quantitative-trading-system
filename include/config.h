#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <unordered_map>
#include "common_types.h"

struct SystemConfig {
    // General settings
    std::string system_name = "TradingSystem";
    bool enable_logging = true;
    bool enable_metrics = true;
    
    // Performance settings
    size_t thread_pool_size = 4;
    size_t queue_capacity = 100000;
    int64_t max_latency_microseconds = 10;
    
    // Market settings
    std::unordered_map<Market, std::string> market_configs = {
        {Market::CHINA_SSE, "sse_config.json"},
        {Market::CHINA_SZSE, "szse_config.json"},
        {Market::HONG_KONG, "hkex_config.json"},
        {Market::USA_NYSE, "nyse_config.json"},
        {Market::USA_NASDAQ, "nasdaq_config.json"}
    };
    
    // Risk settings
    RiskLimits risk_limits;
    
    // Connectivity settings
    std::string fix_endpoint = "localhost:9876";
    int fix_reconnect_interval_ms = 5000;
    int heartbeat_interval_sec = 30;
    
    // Logging settings
    std::string log_level = "INFO";
    std::string log_file = "/var/log/trading_system.log";
    
    // Performance optimization settings
    bool use_memory_pools = true;
    bool enable_cpu_affinity = true;
    int cpu_affinity_core_offset = 0;
};

class ConfigManager {
public:
    static SystemConfig& getInstance() {
        static SystemConfig instance;
        return instance;
    }
    
    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename) const;
    
private:
    ConfigManager() = default;
    ~ConfigManager() = default;
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
};

#endif // CONFIG_H