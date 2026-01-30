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

MIT License# Trading System Architecture Documentation

## System Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           TRADING SYSTEM                                  │
├─────────────────────────────────────────────────────────────────────────────┤
│  ┌─────────────────┐    ┌──────────────────────┐    ┌─────────────────┐   │
│  │   Exchanges     │◄──►│ Connectivity Layer   │◄──►│ Order Management│   │
│  │ (China, HK, US) │    │ - FIX Protocol       │    │ System (OMS)    │   │
│  └─────────────────┘    │ - Auth/Session Mgmt  │    │ - Order Lifecycle│  │
│                         │ - Multi-market supp  │    │ - State tracking │  │
│  ┌─────────────────┐    └──────────────────────┘    └─────────────────┘   │
│  │ Market Data     │                                    ▲                  │
│  │ Feeds          │►►►┐                              │                  │
│  └─────────────────┘   │    ┌─────────────────┐      │    ┌─────────────────┐│
│                        ├───►│ Market Data     │►►►►►►┼────┤ Risk Management ││
│                        │    │ Handler         │      │    │ - Limits        ││
│                        │    │ - Subscriptions │      │    │ - Controls      ││
│                        │    │ - Normalization │      │    │ - Monitoring    ││
│                        │    └─────────────────┘      │    └─────────────────┘│
│                        │                             │                     │
│                        │    ┌─────────────────┐      │                     │
│                        └───►│ Strategy Engine │──────┘                     │
│                             │ - Algorithms    │                           │
│                             │ - Signal Gen    │                           │
│                             │ - Multi-strategy│                           │
│                             └─────────────────┘                           │
│                                  │                                        │
│                                  ▼                                        │
│                         ┌──────────────────────┐                          │
│                         │ Execution Management │                          │
│                         │ System (EMS)         │                          │
│                         │ - Order Routing      │                          │
│                         │ - Exchange Comm      │                          │
│                         │ - Fill Processing    │                          │
│                         └──────────────────────┘                          │
│                                  │                                        │
│                                  ▼                                        │
│                         ┌──────────────────────┐                          │
│                         │ Position & P&L Mgmt  │                          │
│                         │ - Real-time Updates  │                          │
│                         │ - Performance Stats  │                          │
│                         └──────────────────────┘                          │
└─────────────────────────────────────────────────────────────────────────────┘
```

## Data Flow Diagram

```
┌─────────────────┐
│ Market Data     │
│ Feeds           │
│ (Real-time)     │
└───────┬─────────┘
        │
        │ Tick Data (instrument_id, bid/ask, sizes, timestamp)
        │
        ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                     MARKET DATA HANDLER                                     │
├─────────────────────────────────────────────────────────────────────────────┤
│ • Normalize market data formats                                             │
│ • Validate tick data                                                        │
│ • Filter by subscriptions                                                   │
│ • Distribute to subscribers                                                 │
└─────────┬───────────────────────────────────────────────────────────────────┘
          │
          │ Processed Tick Data
          │
          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                        STRATEGY ENGINE                                      │
├─────────────────────────────────────────────────────────────────────────────┤
│ • Run quantitative algorithms                                               │
│ • Process market data                                                       │
│ • Generate trading signals                                                  │
│ • Execute multiple strategies                                               │
└─────────┬───────────────────────────────────────────────────────────────────┘
          │
          │ Trading Signals/Orders
          │
          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                      RISK MANAGEMENT                                        │
├─────────────────────────────────────────────────────────────────────────────┤
│ • Check position limits                                                     │
│ • Validate order values                                                     │
│ • Monitor daily losses                                                      │
│ • Enforce rate limiting                                                     │
└─────────┬───────────────────────────────────────────────────────────────────┘
          │
          │ Validated Orders (or Rejected)
          │
          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                   ORDER MANAGEMENT SYSTEM                                   │
├─────────────────────────────────────────────────────────────────────────────┤
│ • Track order lifecycle                                                     │
│ • Maintain order state                                                      │
│ • Handle modifications/cancellations                                        │
│ • Update order book                                                         │
└─────────┬───────────────────────────────────────────────────────────────────┘
          │
          │ Confirmed Orders
          │
          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                 EXECUTION MANAGEMENT SYSTEM                                 │
├─────────────────────────────────────────────────────────────────────────────┤
│ • Route orders to exchanges                                                 │
│ • Handle FIX protocol communications                                        │
│ • Process acknowledgments/fills                                             │
│ • Manage exchange connections                                               │
└─────────┬───────────────────────────────────────────────────────────────────┘
          │
          │ Fill Reports
          │
          ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                     POSITION & P&L MGMT                                     │
├─────────────────────────────────────────────────────────────────────────────┤
│ • Update positions in real-time                                             │
│ • Calculate profit & loss                                                   │
│ • Generate performance statistics                                           │
│ • Feed back to risk management                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

## Component Interactions

1. **Market Data Handler** receives data from exchanges via the Connectivity Layer and distributes it to the Strategy Engine.

2. **Strategy Engine** processes market data and generates trading signals/orders, sending them to Risk Management.

3. **Risk Management** validates orders against risk parameters and forwards approved orders to Order Management System.

4. **Order Management System** manages the order lifecycle and forwards orders to Execution Management System.

5. **Execution Management System** routes orders to exchanges via the Connectivity Layer and processes fill reports.

6. **Position & P&L Management** updates positions based on fills and feeds back to Risk Management for ongoing monitoring.

## Key Design Principles

- **Modularity**: Each component is independent and can be updated without affecting others
- **Low Latency**: Optimized for microsecond-level performance
- **Scalability**: Can handle multiple markets and instruments simultaneously
- **Fault Tolerance**: Isolated failures don't bring down the entire system
- **Maintainability**: Clean, well-documented code following "less is more" principle