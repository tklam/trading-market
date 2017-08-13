#ifndef _ORDERRING_H
#define _ORDERRING_H


#include <thread>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <sstream>
#include <string>
#include <boost/multiprecision/cpp_dec_float.hpp>

#include "L3/static/disruptor.h"

#ifndef RING_SIZE
#define RING_SIZE (10)
#endif

#ifndef ORDER_BOOK_RING_SIZE
#define ORDER_BOOK_RING_SIZE (15)
#endif

namespace NERV {

enum Action {
    OrderRouting = 1,
    OrderJournaling = 2,
    OrderBookMatching = 4
};

enum OrderType {
    Ask = 1,
    Bid = 2,
    Cancel = 4
};

enum MatchType {
    Market = 1,
    Limit = 2
};

struct Order {
    // order ID of the day which is given out sequentially
    int64_t sequence = 0;
    OrderType type;
    MatchType matchType;
    std::string code; //ASCII representation of the asset
    boost::multiprecision::cpp_dec_float_50 quantity = 0;
    boost::multiprecision::cpp_dec_float_50 price = 0;
    //std::chrono::system_clock::time_point timestamp;
    
    // return human friendly string
    virtual std::string str() const;

    // return a string to be processed by the MatchEngine
    virtual std::string cmdStr() const;

    // populate the fields from a MatchEngine command string
    virtual void fromCmdStr(const std::string & str);
};

struct TradedOrder : public Order {
    boost::multiprecision::cpp_dec_float_50 tradedPrice;

    // return human friendly string
    std::string str() const override;

    // return a string to be processed by the MatchEngine
    std::string cmdStr() const override;

    // populate the fields from a MatchEngine command string
    void fromCmdStr(const std::string & str) override;

    // populate the fields from a MatchEngine command string (Order)
    void fromOrderCmdStr(const std::string & orderStr);
};
}

#endif
