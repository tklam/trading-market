#include "OrderBook.h"
#include <L3/util/scopedtimer.h>

using Clock = std::chrono::steady_clock;

NERV::OrderBook::OrderBook() {
}

NERV::OrderBook::~OrderBook() {
    for (auto & o: _asks) {
        delete o;
    }
    for (auto & o: _bids) {
        delete o;
    }
}

void NERV::OrderBook::addOrder(NERV::Order * o) {
    switch (o->type) {
        case NERV::OrderType::Ask:
            addAskOrder(o);
            ++_totalAsksSize;
            break;
        case NERV::OrderType::Bid:
            addBidOrder(o);
            ++_totalBidsSize;
            break;
        case NERV::OrderType::Cancel:
            //TODO
            break;
    }
}

void NERV::OrderBook::addAskOrder(NERV::Order * o) {
    _asks.push_front(o);
}

void NERV::OrderBook::addBidOrder(NERV::Order * o) {
    _bids.push_front(o);
}

void NERV::OrderBook::sortOrders() {
    using float_typename = boost::multiprecision::cpp_dec_float_50;
    Clock::time_point startTime = Clock::now();

    std::sort (_asks.begin(), _asks.end(), SortAskOrders<float_typename>());
    std::sort (_bids.begin(), _bids.end(), SortBidOrders<float_typename>());

    Clock::time_point endTime = Clock::now();

    std::cout << "[MatchEngine] Time needed to sort asks and bids queues: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count()
              << " ms"
              << " (bid queue size: " << _bids.size()
              << " ask queue size: " << _asks.size() << ")"
              << std::endl;
}

void NERV::OrderBook::match() {
    Clock::time_point startTime = Clock::now();

    auto equal = [&](boost::multiprecision::cpp_dec_float_50 number_1,
                     boost::multiprecision::cpp_dec_float_50 number_2)->bool {
        using float_typename = boost::multiprecision::cpp_dec_float_50;
        if (abs(number_1-number_2) < std::numeric_limits<float_typename>::epsilon()) {
            return true;
        }
        return false;
    };

    auto lessThan= [&](boost::multiprecision::cpp_dec_float_50 number_1,
                       boost::multiprecision::cpp_dec_float_50 number_2)->bool {
        using float_typename = boost::multiprecision::cpp_dec_float_50;
        if ((number_2-number_1) > std::numeric_limits<float_typename>::epsilon()) {
            return true;
        }
        return false;
    };

    //TODO don't really know the logic
    // ask orders: market -> limit, higher price -> lower price
    // bid orders: market -> limit, lower price -> higher price
    for (auto ask_it=_asks.begin(); ask_it!=_asks.end();) {
        NERV::TradedOrder * askOrder = static_cast<NERV::TradedOrder*>(*ask_it);
        bool isMatchAsk = false;

        for (auto bid_it=_bids.begin(); bid_it!=_bids.end();) {
            if (equal(askOrder->quantity, 0)) {
                break;
            }

            NERV::TradedOrder * bidOrder = static_cast<NERV::TradedOrder*>(*bid_it);

            boost::multiprecision::cpp_dec_float_50 tradedQuantity = 0;
            boost::multiprecision::cpp_dec_float_50 tradedPrice = marketPrice;
            bool isMatchBid = false;

            if (   askOrder->matchType == NERV::MatchType::Market
                && bidOrder->matchType == NERV::MatchType::Market) {
                std::cout << "[MatchEngine] Matched: " << std::endl;
                std::cout << "    ask: " << askOrder->cmdStr() << std::endl;       
                std::cout << "    bid: " << bidOrder->cmdStr() << std::endl;       

                tradedQuantity = std::min(askOrder->quantity, bidOrder->quantity);
                askOrder->quantity -= tradedQuantity;
                bidOrder->quantity -= tradedQuantity;
                tradedPrice = marketPrice;
                askOrder->tradedPrice = tradedPrice;
                bidOrder->tradedPrice = tradedPrice;
                isMatchBid = true;
            }
            else if (   askOrder->matchType == NERV::MatchType::Limit
                && bidOrder->matchType == NERV::MatchType::Market) {
                // bid.price must >= ask.price
                if (   lessThan(askOrder->price, bidOrder->price)
                    || equal(askOrder->price, bidOrder->price)) {
                    std::cout << "[MatchEngine] Matched: " << std::endl;
                    std::cout << "    ask: " << askOrder->cmdStr() << std::endl;       
                    std::cout << "    bid: " << bidOrder->cmdStr() << std::endl;       

                    tradedQuantity = std::min(askOrder->quantity, bidOrder->quantity);
                    askOrder->quantity -= tradedQuantity;
                    bidOrder->quantity -= tradedQuantity;
                    tradedPrice = bidOrder->price;
                    askOrder->tradedPrice = tradedPrice;
                    bidOrder->tradedPrice = tradedPrice;
                    marketPrice = tradedPrice;
                    isMatchBid = true;
                }
            }
            else if (   askOrder->matchType == NERV::MatchType::Market
                && bidOrder->matchType == NERV::MatchType::Limit) {
                // ask.price must >= bid.price
                if (   lessThan(bidOrder->price, askOrder->price)
                    || equal(askOrder->price, bidOrder->price)) {
                    std::cout << "[MatchEngine] Matched: " << std::endl;
                    std::cout << "    ask: " << askOrder->cmdStr() << std::endl;       
                    std::cout << "    bid: " << bidOrder->cmdStr() << std::endl;       

                    tradedQuantity = std::min(askOrder->quantity, bidOrder->quantity);
                    askOrder->quantity -= tradedQuantity;
                    bidOrder->quantity -= tradedQuantity;
                    tradedPrice = bidOrder->price;
                    askOrder->tradedPrice = tradedPrice;
                    bidOrder->tradedPrice = tradedPrice;
                    marketPrice = tradedPrice;
                    isMatchBid = true;
                }
            }
            else if (   askOrder->matchType == NERV::MatchType::Limit
                && bidOrder->matchType == NERV::MatchType::Limit) {
                //    ask.price must >= bid.price
                // && bid.price must >= ask.price
                if (equal(askOrder->price, bidOrder->price)) {
                    std::cout << "[MatchEngine] Matched: " << std::endl;
                    std::cout << "    ask: " << askOrder->cmdStr() << std::endl;       
                    std::cout << "    bid: " << bidOrder->cmdStr() << std::endl;       

                    tradedQuantity = std::min(askOrder->quantity, bidOrder->quantity);
                    askOrder->quantity -= tradedQuantity;
                    bidOrder->quantity -= tradedQuantity;
                    tradedPrice = askOrder->price;
                    askOrder->tradedPrice = tradedPrice;
                    bidOrder->tradedPrice = tradedPrice;
                    marketPrice = tradedPrice;
                    isMatchBid = true;
                }
            }

            if (isMatchBid) {
                isMatchAsk = true;
                std::cout << "[MatchEngine] Trade: " << std::endl;
                std::cout << "    tradedPrice: " << tradedPrice << std::endl;       
                std::cout << "    tradedQuantity:  " << tradedQuantity << std::endl;       

                if (equal(bidOrder->quantity, 0)) {
                    bid_it = _bids.erase(bid_it);
                }
                else {
                    ++bid_it;
                }
            }
            else {
                break;
            }
        }

        if (isMatchAsk && equal(askOrder->quantity, 0)) {
            ask_it = _asks.erase(ask_it);
            continue;
        }

        ++ask_it;
    }

    Clock::time_point endTime = Clock::now();

    std::cout << "[MatchEngine] Matching time needed: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count()
              << " ms"
              << " (bid queue size: " << _bids.size()
              << " ask queue size: " << _asks.size() << ")"
              << std::endl;
    std::cout << "totalAsksSize: " << _totalAsksSize
              << " totalBidSize: " << _totalBidsSize << std::endl;
}
