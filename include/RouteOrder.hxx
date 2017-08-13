#include <iostream>
#include <sstream>

template<typename Get>
void RouteOrder<Get>::route(zmq::socket_t & matchEngineReq) {
    //TODO just for proof-of-concept
    size_t i=0;
    bool isLoop = true;
    while (isLoop) {
        for(Order & o: Get())
        {

            s_send (matchEngineReq, o.cmdStr());
           
            /*
            if ((++i)==1000000) {
                isLoop = false;
                return;
            }
            */
        }
    }
    std::cout << "RouteOrder END" << std::endl;
}
