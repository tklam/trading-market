#ifndef _ORDER_BOOK_H_
#define _ORDER_BOOK_H_

#include "OrderRing.h"

#include <deque>
#include <queue>

namespace NERV {

// the order book of an asset
struct OrderBook {
    OrderBook();
    ~OrderBook();

    void addOrder(NERV::Order * o);
    void sortOrders();
    void match();

    boost::multiprecision::cpp_dec_float_50 marketPrice;
private:
    void addBidOrder(NERV::Order * o);
    void addAskOrder(NERV::Order * o);

    std::deque<NERV::Order*> _asks;
    std::deque<NERV::Order*> _bids;

    size_t _totalAsksSize=0;
    size_t _totalBidsSize=0;
};

template<typename Put>
struct FillOrderBook {
    void fill(NERV::Order* o);
};
#include "FillOrderBook.hxx"

template<typename Get>
struct MatchOrderBook {
    void match(OrderBook & orderBook);
};
#include "MatchOrderBook.hxx"

template<typename float_typename>
struct SortBidOrders {
    // bid orders: market -> limit, 
    //             among market orders: order by ascending sequence id
    //             among limit orders: higher price -> lower price
    inline bool operator()(NERV::Order const * a, NERV::Order const * b) {
        if (a->matchType == NERV::MatchType::Market &&
            b->matchType == NERV::MatchType::Limit) {
            return true;
        }
        else if (a->matchType == NERV::MatchType::Limit &&
            b->matchType == NERV::MatchType::Market) {
            return false;
        }            
        else { //either both are of market or limit
            if (a->matchType == NERV::MatchType::Market &&
                b->matchType == NERV::MatchType::Market) {
                return a->sequence < b->sequence;
            }

            float_typename diff = (a->price-b->price);
            if (diff > std::numeric_limits<float_typename>::epsilon()) {
                return true;
            } 
            else if (abs(diff) < std::numeric_limits<float_typename>::epsilon()) {
                return a->sequence < b->sequence;
            }
            else {
                return false;
            }
        }
    }
}; 

template<typename float_typename>
struct SortAskOrders {
    // ask orders: market -> limit 
    //             among market orders: order by ascending sequence id
    //             among limit orders: higher price -> higher price
    inline bool operator()(NERV::Order const * a, NERV::Order const * b) {
        if (a->matchType == NERV::MatchType::Market &&
            b->matchType == NERV::MatchType::Limit) {
            return true;
        }
        else if (a->matchType == NERV::MatchType::Limit &&
            b->matchType == NERV::MatchType::Market) {
            return false;
        }            
        else { //either both are of market or limit
            if (a->matchType == NERV::MatchType::Market &&
                b->matchType == NERV::MatchType::Market) {
                return a->sequence < b->sequence;
            }

            float_typename diff = (a->price-b->price);
            if (diff > std::numeric_limits<float_typename>::epsilon()) {
                return true;
            } 
            else if (abs(diff) < std::numeric_limits<float_typename>::epsilon()) {
                return a->sequence < b->sequence;
            }
            else {
                return false;
            }
        }
    }
};

}
#endif
