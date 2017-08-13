#include <iostream>

template<typename Get>
void MatchOrderBook<Get>::match(OrderBook & orderBook) {
    for(Order * o: Get())
    {
        orderBook.addOrder(o);
    }

    orderBook.sortOrders();
    orderBook.match();
}
