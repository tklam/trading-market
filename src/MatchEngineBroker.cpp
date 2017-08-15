#include "OrderRing.h"
#include "zeromq/zhelpers.hpp"
#include <thread>
#include <unordered_map>
#include <vector>
#include <string>

int main (int argc, char *argv[]) {
    zmq::context_t context(1);

    zmq::socket_t frontend (context, ZMQ_ROUTER);
    frontend.bind("tcp://*:5559");

    zmq::socket_t backend (context, ZMQ_ROUTER);
    backend.bind("tcp://*:5560");

    // asset code ---> list of 0mq identitities
    std::unordered_map<std::string, std::vector<std::string>> workersPerAsset;

    NERV::Order tmpOrder;
    
    // Switch messages between sockets
    while (true) {
        // Initialize poll set
        zmq::pollitem_t items [] = {
            { frontend, 0, ZMQ_POLLIN, 0 },
            { backend,  0, ZMQ_POLLIN, 0 }
        };

        zmq::poll (&items [0], 2, -1);

        
        //received message from client
        if (items [0].revents & ZMQ_POLLIN) {
            //  Process all parts of the message
            std::string clientAddress = s_recv(frontend);
            std::string workload = s_recv(frontend); 

            tmpOrder.fromCmdStr(workload);
            const auto & foundIdentity = workersPerAsset.find(tmpOrder.code);
            auto workerAddresses = foundIdentity->second;
            const std::string & workerAddress = workerAddresses[0]; // TODO

            s_sendmore(backend, workerAddress);
            s_send(backend, workload);
            std::cout << "[MatchEngineBroker] Routing request: " << workload
                      << std::endl;       
        }
        // received message from worker
        if (items [1].revents & ZMQ_POLLIN) {
            std::string workerAddress;
            workerAddress = s_recv(backend);
            
            std::string workload = s_recv(backend); // first hello from worker
            std::cout << "[MatchEngineBroker] hello from worker (" << workerAddress << ")" << ": " << workload << std::endl;
            if (workload == MOTTO) {
                s_recv(backend); //code
                std::string code = s_recv(backend); //code
                workersPerAsset[code].push_back(workerAddress);
                std::cout << "[MatchEngineBroker] registering code: " << code
                          << " handler: " << workerAddress << std::endl;
            }
        }
    }

    return 0;
}

