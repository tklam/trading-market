#include "OrderBook.h"
#include <L3/util/scopedtimer.h>

using Clock = std::chrono::steady_clock;

using TRADED_ORDER_D = L3::Disruptor<NERV::TradedOrder, ORDER_BOOK_RING_SIZE, L3::Tag<NERV::Action::OrderBookCommitTraded>>;
using PUB_TRADED_ORDER_GET = TRADED_ORDER_D::Get<L3::Tag<NERV::Action::OrderBookCommitTraded>>;
using COMMIT_TRADED_ORDER_PUT = TRADED_ORDER_D::Put<L3::Barrier<PUB_TRADED_ORDER_GET>, L3::CommitPolicy::Shared>;

NERV::OrderBook::OrderBook() {
}

NERV::OrderBook::~OrderBook() {
}

void NERV::OrderBook::addOrder(const NERV::Order & o) {
    switch (o.type) {
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

void NERV::OrderBook::addAskOrder(const NERV::Order & o) {
    using SORT_ASK_ORDERS = NERV::SortAskOrders<boost::multiprecision::cpp_dec_float_50>;
    SORT_ASK_ORDERS sorter;
    addOrder_R<SORT_ASK_ORDERS>(o, _asks, 0, _asks.size(), sorter);
    
    //_asks.push_front(o);
}

void NERV::OrderBook::addBidOrder(const NERV::Order & o) {
    using SORT_BID_ORDERS = NERV::SortBidOrders<boost::multiprecision::cpp_dec_float_50>;
    SORT_BID_ORDERS sorter;
    addOrder_R<SORT_BID_ORDERS>(o, _bids, 0, _bids.size(), sorter);

    //_bids.push_front(o);
}

void NERV::OrderBook::sortOrders() {
    using float_typename = boost::multiprecision::cpp_dec_float_50;

    std::sort (_asks.begin(), _asks.end(), SortAskOrders<float_typename>());
    std::sort (_bids.begin(), _bids.end(), SortBidOrders<float_typename>());
}

void NERV::OrderBook::match() {
    Clock::time_point startTime = Clock::now();
    
    NERV::CommitTradedOrder<COMMIT_TRADED_ORDER_PUT> commitAction;

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
    // bid orders: market -> limit, higher price -> lower price
    for (auto ask_it=_asks.begin(); ask_it!=_asks.end();) {
        NERV::Order & askOrder = (*ask_it);
        bool isMatchAsk = false;

        for (auto bid_it=_bids.begin(); bid_it!=_bids.end();) {
            if (equal(askOrder.quantity, 0)) {
                break;
            }

            NERV::Order & bidOrder = (*bid_it);

            boost::multiprecision::cpp_dec_float_50 tradedQuantity = 0;
            bool isMatchBid = false;

            if (   askOrder.matchType == NERV::MatchType::Market
                && bidOrder.matchType == NERV::MatchType::Market) {
                tradedQuantity = std::min(askOrder.quantity, bidOrder.quantity);

                NERV::TradedOrder tradedOrder;
                tradedOrder.askOrder = askOrder;
                tradedOrder.bidOrder = bidOrder;
                tradedOrder.tradedQuantity = tradedQuantity;
                tradedOrder.tradedPrice = marketPrice;
                commitAction.commit(tradedOrder);
                std::cout << tradedOrder.str() << std::endl;

                askOrder.quantity -= tradedQuantity;
                bidOrder.quantity -= tradedQuantity;
                isMatchBid = true;
            }
            else if (   askOrder.matchType == NERV::MatchType::Limit
                && bidOrder.matchType == NERV::MatchType::Market) {
                // bid.price must >= ask.price
                if (   lessThan(askOrder.price, bidOrder.price)
                    || equal(askOrder.price, bidOrder.price)) {
                    tradedQuantity = std::min(askOrder.quantity, bidOrder.quantity);

                    NERV::TradedOrder tradedOrder;
                    tradedOrder.askOrder = askOrder;
                    tradedOrder.bidOrder = bidOrder;
                    tradedOrder.tradedQuantity = tradedQuantity;
                    tradedOrder.tradedPrice = bidOrder.price;
                    commitAction.commit(tradedOrder);
                    std::cout << tradedOrder.str() << std::endl;

                    askOrder.quantity -= tradedQuantity;
                    bidOrder.quantity -= tradedQuantity;
                    marketPrice = tradedOrder.tradedPrice;
                    isMatchBid = true;
                }
            }
            else if (   askOrder.matchType == NERV::MatchType::Market
                && bidOrder.matchType == NERV::MatchType::Limit) {
                // ask.price must >= bid.price
                if (   lessThan(bidOrder.price, askOrder.price)
                    || equal(askOrder.price, bidOrder.price)) {
                    tradedQuantity = std::min(askOrder.quantity, bidOrder.quantity);

                    NERV::TradedOrder tradedOrder;
                    tradedOrder.askOrder = askOrder;
                    tradedOrder.bidOrder = bidOrder;
                    tradedOrder.tradedQuantity = tradedQuantity;
                    tradedOrder.tradedPrice = bidOrder.price;
                    commitAction.commit(tradedOrder);
                    std::cout << tradedOrder.str() << std::endl;

                    askOrder.quantity -= tradedQuantity;
                    bidOrder.quantity -= tradedQuantity;
                    marketPrice = tradedOrder.tradedPrice;
                    isMatchBid = true;
                }
            }
            else if (   askOrder.matchType == NERV::MatchType::Limit
                && bidOrder.matchType == NERV::MatchType::Limit) {
                //    ask.price must >= bid.price
                // && bid.price must >= ask.price
                if (equal(askOrder.price, bidOrder.price)) {
                    tradedQuantity = std::min(askOrder.quantity, bidOrder.quantity);

                    NERV::TradedOrder tradedOrder;
                    tradedOrder.askOrder = askOrder;
                    tradedOrder.bidOrder = bidOrder;
                    tradedOrder.tradedQuantity = tradedQuantity;
                    tradedOrder.tradedPrice = bidOrder.price;
                    commitAction.commit(tradedOrder);
                    std::cout << tradedOrder.str() << std::endl;

                    askOrder.quantity -= tradedQuantity;
                    bidOrder.quantity -= tradedQuantity;
                    marketPrice = tradedOrder.tradedPrice;
                    isMatchBid = true;
                }
            }

            if (isMatchBid) {
                isMatchAsk = true;

                if (equal(bidOrder.quantity, 0)) {
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

        if (isMatchAsk && equal(askOrder.quantity, 0)) {
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
