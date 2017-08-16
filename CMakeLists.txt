cmake_minimum_required(VERSION 2.6)

#project info
project(TradingMarket)

#set version
set(TradingMarket_VERSION_MAJOR 1)
set(TradingMarket_VERSION_MINOR 0)

#set environment variables
message("CMAKE_CURRENT_SOURCE_DIR = " ${CMAKE_CURRENT_SOURCE_DIR})
message("CMAKE_CURRENT_BINARY_DIR = " ${CMAKE_CURRENT_BINARY_DIR})
message("CMAKE_INSTALL_PREFIX = " ${CMAKE_INSTALL_PREFIX})

add_subdirectory(./lib/cppzmq)

#headers
include_directories (. ./include ./lib/cppzmq )

set(CMAKE_CXX_FLAGS -std=c++14)
set(global_optimization_flag -O3)

#library directory
add_definitions(${global_optimization_flag})

#config of OrderRingServer
ADD_DEFINITIONS(-DRING_SIZE=10 -DORDER_BOOK_RING_SIZE=12)

set(LIB_SOURCE 
    src/OrderRing.cpp
    src/OrderBook.cpp
)
add_library(libTradingMarket
    ${LIB_SOURCE}
)
target_link_libraries(libTradingMarket boost_filesystem boost_system)

# change lib_target properties
SET_TARGET_PROPERTIES( libTradingMarket PROPERTIES
# create *nix style library versions + symbolic links
    VERSION ${${PROJECT_NAME}_VERSION}
    SOVERSION ${${PROJECT_NAME}_SOVERSION}
    # allow creating static and shared libs without conflicts
    CLEAN_DIRECT_OUTPUT 1
    # avoid conflicts between library and binary target names
    OUTPUT_NAME TradingMarket)


# OrderRingServer
add_executable(orderRingServer
    src/OrderRingServer.cpp
)
target_link_libraries(orderRingServer libTradingMarket pthread zmq)

# MatchEngineBroker
add_executable(matchEngineBroker
    src/MatchEngineBroker.cpp
)
target_link_libraries(matchEngineBroker libTradingMarket pthread zmq)

# MatchEngineService
add_executable(matchEngineService
    src/MatchEngineService.cpp
)
target_link_libraries(matchEngineService libTradingMarket pthread zmq)

INSTALL(TARGETS orderRingServer libTradingMarket
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
