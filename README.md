# High-Performance Trading System

A low-latency, modular trading system implemented in C++ for quantitative trading strategies with focus on China, Hong Kong, and USA markets.

## Features

- **Low-latency design**: Optimized for microseconds-level performance
- **Modular architecture**: Independent components that can be updated without affecting others
- **Multi-market support**: Designed for China (SSE, SZSE), Hong Kong (HKEX), and USA (NYSE, NASDAQ) markets
- **Risk management**: Real-time risk controls and limits
- **Strategy engine**: Pluggable algorithmic trading strategies
- **C++17**: Leveraging modern C++ features for performance

## Architecture

The system consists of the following core modules:

1. **Market Data Handler**: Processes real-time market data feeds
2. **Order Management System (OMS)**: Manages order lifecycle
3. **Execution Management System (EMS)**: Handles order routing to exchanges
4. **Risk Management**: Implements real-time risk controls
5. **Strategy Engine**: Runs trading algorithms and generates signals
6. **Connectivity Layer**: Manages connections to exchanges

## Requirements

- C++17 compiler (GCC 7+, Clang 5+, or MSVC 2017+)
- CMake 3.16+
- POSIX-compliant OS (Linux/macOS recommended)

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

```bash
./trading_system
```

## Design Philosophy

- **"Less is more"**: Clean, maintainable code with minimal complexity
- **Performance first**: Optimized for speed without sacrificing readability
- **Modular design**: Components can be replaced or updated independently
- **Real-world focus**: Built with actual trading requirements in mind

## License

MIT License