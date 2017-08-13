#include <iostream>

template<typename Put>
void FillOrderBook<Put>::fill(NERV::Order * o) {
    Put() = o;
}
