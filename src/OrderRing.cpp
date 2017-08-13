#include "OrderRing.h"
#include <boost/tokenizer.hpp>

std::string NERV::Order::str() const {
    std::stringstream s;
    s << "Order #" << sequence
      << " type: " << type
      << " matchType: " << matchType
      << " code: " << code
      << " quantity: " << quantity
      << " price: " << price; 
    return s.str();
}

std::string NERV::Order::cmdStr() const {
    std::stringstream s;
    s << "ORDER " << sequence;

    s << " ";
    switch (type) {
        case NERV::OrderType::Ask:
            s << "ASK";
            break;
        case NERV::OrderType::Bid:
            s << "BID";
            break;
        case NERV::OrderType::Cancel:
            s << "CAN";
            break;
    }

    s << " ";
    switch (matchType) {
        case NERV::MatchType::Market:
            s << "MARKET";
            break;
        case NERV::MatchType::Limit:
            s << "LIMIT";
            break;
    }

    s << " ";
    s << code;

    s << " ";
    s << quantity.str(50);

    s << " ";
    s << price.str(); 
    return s.str();
}

void NERV::Order::fromCmdStr(const std::string & str) {
    using tokenizer = boost::tokenizer<boost::char_separator<char> > ;
    boost::char_separator<char> sep(" ");
    tokenizer tokens(str, sep);
    auto tok_iter = tokens.begin();

    // PREFIX
    if (*tok_iter != "ORDER") {
        throw std::invalid_argument( "Not an Order command string: incorrect prefix" );
    }
    ++tok_iter;

    // sequence
    sequence = std::stoi(*tok_iter);
    ++tok_iter;

    // type
    if (*tok_iter == "ASK") {
        type = NERV::OrderType::Ask;
    }
    else if (*tok_iter == "BID") {
        type = NERV::OrderType::Bid;
    }
    else if (*tok_iter == "CAN") {
        type = NERV::OrderType::Cancel;
    }
    else {
        throw std::invalid_argument( "Not an Order command string: incorrect type" );
    }
    ++tok_iter;

    // matchType
    if (*tok_iter == "MARKET") {
        matchType = NERV::MatchType::Market;
    }
    else if (*tok_iter == "LIMIT") {
        matchType = NERV::MatchType::Limit;
    }
    else {
        throw std::invalid_argument( "Not an Order command string: incorrect matchType" );
    }
    ++tok_iter;

    code = *tok_iter;
    ++tok_iter;
    
    quantity = boost::multiprecision::cpp_dec_float_50(*tok_iter);
    ++tok_iter;

    price = boost::multiprecision::cpp_dec_float_50(*tok_iter);
    ++tok_iter;
}

std::string NERV::TradedOrder::str() const {
    std::stringstream s;
    s << "Order #" << sequence
      << " type: " << type
      << " matchType: " << matchType
      << " code: " << code
      << " quantity: " << quantity
      << " price: " << price 
      << " tradedPrice: " << tradedPrice; 
    return s.str();
}

std::string NERV::TradedOrder::cmdStr() const {
    std::stringstream s;
    s << "ORDER " << sequence;

    s << " ";
    switch (type) {
        case NERV::OrderType::Ask:
            s << "ASK";
            break;
        case NERV::OrderType::Bid:
            s << "BID";
            break;
        case NERV::OrderType::Cancel:
            s << "CAN";
            break;
    }

    s << " ";
    switch (matchType) {
        case NERV::MatchType::Market:
            s << "MARKET";
            break;
        case NERV::MatchType::Limit:
            s << "LIMIT";
            break;
    }

    s << " ";
    s << code;

    s << " ";
    s << quantity.str(50);

    s << " ";
    s << price.str(); 

    s << " ";
    s << tradedPrice.str(); 
    return s.str();
}

void NERV::TradedOrder::fromCmdStr(const std::string & str) {
    using tokenizer = boost::tokenizer<boost::char_separator<char> > ;
    boost::char_separator<char> sep(" ");
    tokenizer tokens(str, sep);
    auto tok_iter = tokens.begin();

    // PREFIX
    if (*tok_iter != "ORDER") {
        throw std::invalid_argument( "Not an Order command string: incorrect prefix" );
    }
    ++tok_iter;

    // sequence
    sequence = std::stoi(*tok_iter);
    ++tok_iter;

    // type
    if (*tok_iter == "ASK") {
        type = NERV::OrderType::Ask;
    }
    else if (*tok_iter == "BID") {
        type = NERV::OrderType::Bid;
    }
    else if (*tok_iter == "CAN") {
        type = NERV::OrderType::Cancel;
    }
    else {
        throw std::invalid_argument( "Not an Order command string: incorrect type" );
    }
    ++tok_iter;

    // matchType
    if (*tok_iter == "MARKET") {
        matchType = NERV::MatchType::Market;
    }
    else if (*tok_iter == "LIMIT") {
        matchType = NERV::MatchType::Limit;
    }
    else {
        throw std::invalid_argument( "Not an Order command string: incorrect matchType" );
    }
    ++tok_iter;

    code = *tok_iter;
    ++tok_iter;
    
    quantity = boost::multiprecision::cpp_dec_float_50(*tok_iter);
    ++tok_iter;

    price = boost::multiprecision::cpp_dec_float_50(*tok_iter);
    ++tok_iter;

    tradedPrice = boost::multiprecision::cpp_dec_float_50(*tok_iter);
    ++tok_iter;
}

void NERV::TradedOrder::fromOrderCmdStr(const std::string & orderStr) {
    NERV::Order::fromCmdStr(orderStr);
    tradedPrice = 0;
}
