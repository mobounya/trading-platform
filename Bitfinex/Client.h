#pragma once

#include <Bitfinex/OrderBook.h>
#include <Bitfinex/ENUMS.h>
#include <dotenv/dotenv.h>
#include <optional>
#include <string>
#include <utility>

namespace Bitfinex {

struct TickerResponse {
    unsigned short http_status;
    double last_price; // Price of the last trade
    double bid; // Price of last highest bid
    double volume; // Daily volume
};

struct OrderResponse {
    std::string type;
    unsigned short http_status;
    unsigned long order_id;
    std::string message;
    std::string symbol;
    OrderSide side;
    double amount;
    double price;
};

struct IncreasePositionResponse {
    unsigned short http_status;
    std::string message;
};

struct Order {
    std::string order_id;
    OrderSide side;
    std::string symbol;
    double amount;
    OrderType type;
    double price;
    std::string creation_date;
};

struct Config {
    std::string BASE_ENDPOINT;
    std::string API_KEY;
    std::string SECRET_KEY;
};

class Client {
public:
    explicit Client(Config const& config) : m_config(config) {}
    static TickerResponse get_ticker(std::string const&);
    OrderResponse submit_order(Order const&);
    OrderResponse update_order(std::string const& order_id, double price);
    OrderResponse cancel_order(std::string const&);
    std::optional<OrderBook> retrieve_orders(std::string const&);
    IncreasePositionResponse increase_position(PositionSide, std::string const& symbol, double amount);
private:
    const Config m_config;
};

std::string get_current_timestamp_as_string();
std::string unix_to_iso_utc(long long milliseconds_unix_time_stamp);
std::string hex_hmac_sha384(const std::string& key, const std::string& data);
std::ostream& operator<<(std::ostream& cin, Order const&);
}
