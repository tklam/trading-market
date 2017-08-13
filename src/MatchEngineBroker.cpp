#include "zeromq/zhelpers.hpp"
#include <thread>

int main (int argc, char *argv[]) {
    zmq::context_t context(1);

    zmq::socket_t frontend (context, ZMQ_ROUTER);
    frontend.bind("tcp://*:5559");

    zmq::socket_t backend (context, ZMQ_ROUTER);
    backend.bind("tcp://*:5560");



    std::string workerAddress;
    
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
            std::string clientRequest= s_recv(frontend);

            s_sendmore(backend, workerAddress);
            s_send(backend, clientRequest);
            std::cout << "[MatchEngineBroker] Routing request: " << clientRequest
                      << std::endl;       
        }
        // received message from worker
        if (items [1].revents & ZMQ_POLLIN) {
            workerAddress = s_recv(backend);
            s_recv(backend);     // Envelope delimiter
            std::string workload = s_recv(backend);     //  Response from worker
            std::cout << "[MatchEngineBroker] workload from worker (" << workerAddress << ")" << ": " << workload << std::endl;
        }
    }

    return 0;
}

