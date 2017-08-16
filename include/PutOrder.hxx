#include <iostream>
#include <cstdlib>

template<typename Put>
void PutOrder<Put>::put() {
    //TODO just for proof-of-concept
    //while (true) {
    
    boost::multiprecision::cpp_dec_float_50 prevPrice = 0;
    boost::multiprecision::cpp_dec_float_50 spread = 0.0001;

    for (auto i=0; i<1000000; ++i) {
        Order order;
        int r = (rand() % 3);
        switch (r) {
            case 0:
                order.type = NERV::OrderType::Ask;
                break;
            case 1:
                order.type = NERV::OrderType::Bid;
                break;
            case 2:
                order.type = NERV::OrderType::Cancel;
                break;
        }
        r = (rand() % 2);
        switch (r) {
            case 0:
                order.matchType = NERV::MatchType::Market;
                break;
            case 1:
                order.matchType = NERV::MatchType::Limit;
                break;
        }
        order.sequence = sequence++;
        order.quantity= (rand() % 10000)/boost::multiprecision::cpp_dec_float_50(10000.0)+1;
        if (i == 0) {
            order.price = (rand()% 10000)/boost::multiprecision::cpp_dec_float_50(10000.0)+1;
        }
        else {
            int dir = rand() % 3;
            if (dir == 0) {
                order.price = prevPrice + spread;
            }
            else if (dir == 1) {
                if (prevPrice > 0) {
                    order.price = prevPrice - spread;
                }
            }
            else {
                order.price = prevPrice;
            }
        }
        prevPrice = order.price;
        order.code = "TKLAM";
        //order.timestamp = std::chrono::system_clock::now();
        Put() = order;
        std::cout << "Produced Order #" << sequence << std::endl;
    }
    std::cout << "PutOrder END" << std::endl;
}
