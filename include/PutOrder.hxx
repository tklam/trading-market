#include <iostream>
#include <cstdlib>

template<typename Put>
void PutOrder<Put>::put() {
    //TODO just for proof-of-concept
    //while (true) {
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
        order.price = (rand()% 10000)/boost::multiprecision::cpp_dec_float_50(10000.0)+1;
        order.code = "TKLAM";
        //order.timestamp = std::chrono::system_clock::now();
        Put() = order;
        std::cout << "Produced Order #" << sequence << std::endl;
    }
    std::cout << "PutOrder END" << std::endl;
}
