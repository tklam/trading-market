#ifndef _ROUTE_ORDER_H_
#define _ROUTE_ORDER_H_

#include "OrderRing.h"
#include "zeromq/zhelpers.hpp"

namespace NERV {

template<typename Get>
struct RouteOrder{
    void route(zmq::socket_t & matchEngineReq);
};

#include "RouteOrder.hxx"

};

#endif
