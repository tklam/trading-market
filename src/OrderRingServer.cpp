#include "RouteOrder.h"
#include "PutOrder.h"
#include "JournalOrder.h"
#include "zeromq/zhelpers.hpp"

int main(int argc, char** argv) {
    // setup MatchEngine clients
    zmq::context_t context(1);
    zmq::socket_t matchEngineSocket(context, ZMQ_DEALER);
    matchEngineSocket.connect("tcp://localhost:5559"); // address of the MatchEngine broker

    // setup OrderRing server
    using ORDER_D = L3::Disruptor<NERV::Order, RING_SIZE>;
    using JOURNAL_ORDER_GET = ORDER_D::Get<L3::Tag<NERV::Action::OrderJournaling>>;
    using ROUTE_ORDER_GET = ORDER_D::Get<L3::Tag<NERV::OrderRouting>, L3::Barrier<JOURNAL_ORDER_GET>>;
    using PUT_ORDER_PUT = ORDER_D::Put<L3::Barrier<ROUTE_ORDER_GET>, L3::CommitPolicy::Shared>;

    std::cout << "OrderRing server with ring size: " << RING_SIZE << std::endl;
    NERV::PutOrder<PUT_ORDER_PUT> putOrder;
    NERV::RouteOrder<ROUTE_ORDER_GET> routeOrder;
    NERV::JournalOrder<JOURNAL_ORDER_GET> journalOrder;
    
    std::thread putOrder_t([&]{putOrder.put();});
    std::thread routeOrder_t([&]{routeOrder.route(matchEngineSocket);});
    std::thread journalOrder_t([&]{journalOrder.save();});

    putOrder_t.join();
    routeOrder_t.join();
    journalOrder_t.join();

    return 0;
}
