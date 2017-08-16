#include <iostream>

using Clock = std::chrono::steady_clock;

template<typename Get>
void MatchOrderBook<Get>::match(OrderBook & orderBook) {
    Clock::time_point startTime = Clock::now();

    for(const Order & o: Get())
    {
        orderBook.addOrder(o);
    }

    orderBook.match();

    Clock::time_point endTime = Clock::now();

    std::cout << "[MatchEngine] Time needed to add and match a batch of orders: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count()
              << " ms"
              << std::endl;
}
