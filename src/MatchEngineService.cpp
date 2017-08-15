#include "zeromq/zhelpers.hpp"
#include "MatchEngine.h"
#include "OrderBook.h"

int main (int argc, char *argv[]) {
    NERV::OrderBook orderBook;

    using ORDER_D = L3::Disruptor<NERV::Order*, ORDER_BOOK_RING_SIZE>;
    using MATCH_ORDER_BOOK_GET = ORDER_D::Get<L3::Tag<NERV::Action::OrderBookMatching>>;
    using FILL_ORDER_BOOK_PUT = ORDER_D::Put<L3::Barrier<MATCH_ORDER_BOOK_GET>, L3::CommitPolicy::Shared>;
    NERV::FillOrderBook<FILL_ORDER_BOOK_PUT> fillOrderBook;
    NERV::MatchOrderBook<MATCH_ORDER_BOOK_GET> matchOrderBook;

    zmq::context_t context(1);
    zmq::socket_t matchEngineBrokerSocket(context, ZMQ_DEALER);
    matchEngineBrokerSocket.connect("tcp://localhost:5560");

    // just to say hi to MatchEngineBroker
    // 1. MOTTO
    // 2. asset code
    s_send(matchEngineBrokerSocket, MOTTO); 
    s_send(matchEngineBrokerSocket, "TKLAM");

    std::thread matchOrderBook_t(
        [&]{
            while (true) {
                matchOrderBook.match(orderBook);
            }
        }
    );

    std::thread waitRequest_t(
        [&]{
            while(true) {
                //  Wait for next request from client
                std::string request = s_recv (matchEngineBrokerSocket);
                NERV::TradedOrder * o = new NERV::TradedOrder();
                o->fromOrderCmdStr(request);
                fillOrderBook.fill(o);
                
                /*
                std::cout << "[MatchEngine] Enqueue request: " << std::endl;
                std::cout << "    request: " << request << std::endl;       
                std::cout << "    cmdStr:  " << o->cmdStr() << std::endl;       
                */
            }
        }
    );

    matchOrderBook_t.join();
    waitRequest_t.join();
}
