#include <Bitfinex/ENUMS.h>
#include <cassert>
#include <boost/algorithm/string.hpp>

namespace Bitfinex {

bool is_valid_trade_mode(std::string const& str)
{
    return (str == "cross" || str == "isolated" || str == "cash" || str == "spot_isolated");
}

TradeMode trade_mode_from_string(std::string const& str)
{
    assert(is_valid_trade_mode(str));

    if (str == "cross") return TradeMode::CROSS;
    if (str == "isolated") return TradeMode::ISOLATED;
    if (str == "cash") return TradeMode::CASH;
    else
        return TradeMode::SPOT_ISOLATED;
}

std::string trade_mode_to_string(TradeMode trade_mode)
{
    switch (trade_mode) {
    case TradeMode::CASH:
        return "cash";
    case TradeMode::SPOT_ISOLATED:
        return "spot_isolated";
    case TradeMode::ISOLATED:
        return "isolated";
    case TradeMode::CROSS:
        return "cross";
    }
}

bool is_valid_order_side(std::string const& str)
{
    return (str == "sell" || str == "buy");
}

OrderSide order_side_from_string(std::string const& str)
{
    assert(is_valid_order_side(str));

    if (str == "sell")
        return OrderSide::SELL;
    else
        return OrderSide::BUY;
}

std::string order_side_to_string(OrderSide side)
{
    switch (side) {
    case OrderSide::SELL:
        return "sell";
    case OrderSide::BUY:
        return "buy";
    }
}

bool is_valid_order_type(std::string const& str)
{
    return (str == "limit" || str == "exchange_limit" || str == "market" || str == "exchange_market" || str == "stop" || str == "exchange_stop"
                || str == "stop_limit" || str == "exchange_stop_limit" || str == "trailing_stop" || str == "exchange_trailing_stop" || str == "fill_or_kill"
                    || str == "fok" || str == "exchange_fok" || str == "immediate_or_cancel" || str == "ioc" || str == "exchange_ioc");
}

OrderType order_type_from_string(std::string const& str)
{
    std::string lowercase_str = boost::algorithm::to_lower_copy(str);
    assert(is_valid_order_type(lowercase_str));

    if (lowercase_str == "limit") return OrderType::LIMIT;
    if (lowercase_str == "exchange_limit") return OrderType::EXCHANGE_LIMIT;
    if (lowercase_str == "market") return OrderType::MARKET;
    if (lowercase_str == "exchange_market") return OrderType::EXCHANGE_MARKET;
    if (lowercase_str == "stop") return OrderType::STOP;
    if (lowercase_str == "exchange_stop") return OrderType::EXCHANGE_STOP;
    if (lowercase_str == "stop_limit") return OrderType::STOP_LIMIT;
    if (lowercase_str == "exchange_stop_limit") return OrderType::EXCHANGE_STOP_LIMIT;
    if (lowercase_str == "trailing_stop") return OrderType::TRAILING_STOP;
    if (lowercase_str == "exchange_trailing_stop") return OrderType::EXCHANGE_TRAILING_STOP;
    if (lowercase_str == "fill_or_kill" || lowercase_str == "fok") return OrderType::FILL_OR_KILL;
    if (lowercase_str == "exchange_fok") return OrderType::EXCHANGE_FOK;
    if (lowercase_str == "immediate_or_cancel" || lowercase_str == "ioc") return OrderType::IMMEDIATE_OR_CANCEL;
    else
        return OrderType::EXCHANGE_IOC;
}

std::string order_type_to_string(OrderType type)
{
    switch (type) {
    case OrderType::LIMIT:
        return "LIMIT";
    case OrderType::EXCHANGE_LIMIT:
        return "EXCHANGE LIMIT";
    case OrderType::MARKET:
        return "MARKET";
    case OrderType::EXCHANGE_MARKET:
        return "EXCHANGE MARKET";
    case OrderType::STOP:
        return "STOP";
    case OrderType::EXCHANGE_STOP:
        return "EXCHANGE STOP";
    case OrderType::STOP_LIMIT:
        return "STOP LIMIT";
    case OrderType::EXCHANGE_STOP_LIMIT:
        return "EXCHANGE STOP LIMIT";
    case OrderType::TRAILING_STOP:
        return "TRAILING STOP";
    case OrderType::EXCHANGE_TRAILING_STOP:
        return "EXCHANGE TRAILING STOP";
    case OrderType::FILL_OR_KILL:
        return "FOK";
    case OrderType::EXCHANGE_FOK:
        return "EXCHANGE FOK";
    case OrderType::IMMEDIATE_OR_CANCEL:
        return "IOC";
    case OrderType::EXCHANGE_IOC:
        return "EXCHANGE IOC";
    }
}

}
