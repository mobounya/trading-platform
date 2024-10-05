#pragma once

#include <cstdint>
#include <string>

namespace Bitfinex {

enum TradeMode : uint8_t {
    CROSS,
    ISOLATED,
    CASH,
    SPOT_ISOLATED
};

enum OrderSide : uint8_t {
    BUY,
    SELL
};

enum PositionSide : uint8_t {
    SHORT,
    LONG
};

enum OrderType : uint8_t {
    LIMIT,
    EXCHANGE_LIMIT,
    MARKET,
    EXCHANGE_MARKET,
    STOP,
    EXCHANGE_STOP,
    STOP_LIMIT,
    EXCHANGE_STOP_LIMIT,
    TRAILING_STOP,
    EXCHANGE_TRAILING_STOP,
    FILL_OR_KILL, // fok
    EXCHANGE_FOK,
    IMMEDIATE_OR_CANCEL, // ioc
    EXCHANGE_IOC,
};

bool is_valid_trade_mode(std::string const&);
TradeMode trade_mode_from_string(std::string const&);
std::string trade_mode_to_string(TradeMode);

bool is_valid_order_side(std::string const&);
OrderSide order_side_from_string(std::string const&);
std::string order_side_to_string(OrderSide);

bool is_valid_order_type(std::string const&);
OrderType order_type_from_string(std::string const&);
std::string order_type_to_string(OrderType);

bool is_valid_position_side(std::string const&);
PositionSide position_side_from_string(std::string const&);

}
