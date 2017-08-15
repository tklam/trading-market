Requirement
-----------

This project depends on the following libraries:

+ zeromq version 3
+ [L3](https://github.com/abwilson/L3)
+ [cppzmq](https://github.com/zeromq/cppzmq)
+ boost::multiprecision


Architecture
------------
```
    OrderRingServer
    
    +--------------+    +--------+
    |LMAX Disruptor|<---|PutOrder|
    +--------------+    +--------+
      |
      |    +------------+    +----------+             +----------------+
      +--->|JournalOrder|--->|RouteOrder|---> 0mq --->|MatchOrderBroker|
           +------------+    +----------+             +----------------+

    0mq communication protocol:
    RouteOrder -> MatchOrderBroker: order command strng



    MatchOrderBroker

    +----------------+             +----------------------------------------------------+
    |MatchOrderBroker|<--- 0mq --->|MatchOrderService (1..n, one service for each asset)|
    +----------------+             +----------------------------------------------------+

    0mq communication protocol:
      Initialization:
        1. MatchOrderService -> MatchOrderBroker: "Caelum non animum muto"
        2. MatchOrderService -> MatchOrderBroker: "<asset code>"
      Return result:
        2. MatchOrderService -> MatchOrderBroker: "<order status>"

```
