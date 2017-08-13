#ifndef _PUT_ORDER_H_
#define _PUT_ORDER_H_

#include "OrderRing.h"

namespace NERV {

template<typename Put>
struct PutOrder {
    size_t sequence = 1;
    void put();
};

#include "PutOrder.hxx"

};


#endif
