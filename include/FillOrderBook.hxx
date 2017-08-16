#include <iostream>

template<typename Put>
void FillOrderBook<Put>::fill(const std::string & orderCmdStr) {
    static NERV::Order orderTemplate;
    orderTemplate.fromCmdStr(orderCmdStr);
    Put() = orderTemplate;
}
