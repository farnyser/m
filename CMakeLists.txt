cmake_minimum_required(VERSION 3.8)
project(m)

set(CMAKE_CXX_STANDARD 17)
include_directories(.)

set(CORE_SOURCE_FILES Core/Order.hpp Core/Type.hpp Core/InstrumentId.hpp Core/Price.hpp Core/Quantity.hpp Core/Direction.hpp Core/OrderBook.hpp)
set(SOURCE_FILES main.cpp ${CORE_SOURCE_FILES})
set(TEST_SOURCE_FILES tests.cpp ${CORE_SOURCE_FILES})

add_executable(m ${SOURCE_FILES})
add_executable(tests ${TEST_SOURCE_FILES})