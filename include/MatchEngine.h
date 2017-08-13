#ifndef _MATCH_ENGINE_H_
#define _MATCH_ENGINE_H_

#include <string>
#include <deque>
#include "OrderRing.h"

struct MatchEngine {
    void addAsk(NERV::Order & order);
    void addBid(NERV::Order & order);

private:
    std::deque<NERV::Order *> _asks;
    std::deque<NERV::Order *> _buys;
};

#endif
