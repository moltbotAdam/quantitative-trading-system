cmake_minimum_required(VERSION 3.16)
project(TradingSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -DNDEBUG -Wall -Wextra")

# Find required packages
find_package(Threads REQUIRED)

# Include directories
include_directories(include)

# Define source files
set(SOURCES
    src/main.cpp
    src/market_data_handler.cpp
    src/order_management_system.cpp
    src/execution_management_system.cpp
    src/risk_management.cpp
    src/strategy_engine.cpp
    src/connectivity_layer.cpp
)

# Define header files
set(HEADERS
    include/market_data_handler.h
    include/order_management_system.h
    include/execution_management_system.h
    include/risk_management.h
    include/strategy_engine.h
    include/connectivity_layer.h
    include/common_types.h
    include/config.h
)

# Create executable
add_executable(trading_system ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(trading_system Threads::Threads)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(trading_system PRIVATE -flto)
    set_property(TARGET trading_system PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# Installation
install(TARGETS trading_system DESTINATION bin)