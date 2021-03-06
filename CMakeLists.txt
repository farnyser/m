cmake_minimum_required(VERSION 3.8)
project(m)

set(CMAKE_CXX_STANDARD 17)
include_directories(.)

set(CORE_SOURCE_FILES Core/Order.hpp Core/Type.hpp Core/InstrumentId.hpp Core/Price.hpp Core/Quantity.hpp Core/Direction.hpp Core/Fulfillment.h Core/TImeInForce.hpp Core/Id.hpp Core/OrderStatus.hpp)
set(ME_SOURCE_FILES MatchingEngine/MatchingOrder.hpp MatchingEngine/OrderBook.hpp MatchingEngine/OrderBook.Builder.hpp MatchingEngine/Helper.hpp)
set(TOOLING_FILES Tests/Assert.hpp)

set(SOURCE_FILES main.cpp ${CORE_SOURCE_FILES} ${ME_SOURCE_FILES} ${TOOLING_FILES})
set(TEST_SOURCE_FILES tests.cpp ${CORE_SOURCE_FILES} ${ME_SOURCE_FILES} ${TOOLING_FILES} Tests/PlaceUpdateCancelTests.hpp Tests/AssertExtension.hpp)

add_executable(m ${SOURCE_FILES})
add_executable(tests ${TEST_SOURCE_FILES})