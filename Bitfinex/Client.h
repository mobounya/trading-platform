#pragma once

#include <Bitfinex/ENUMS.h>
#include <dotenv/dotenv.h>
#include <optional>
#include <string>
#include <utility>

namespace Bitfinex {

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

struct Order {
    std::string order_id;
    OrderSide side;
    std::string symbol;
    double amount;
    OrderType type;
    double price;
};

struct Config {
    std::string BASE_ENDPOINT;
    std::string API_KEY;
    std::string SECRET_KEY;
};

class MalFormedOrderException : public std::exception {
public:
    MalFormedOrderException(const char *error_message) : m_error_message(error_message) {}
    [[nodiscard]] const char* what() const noexcept override { return m_error_message; }
private:
    const char *m_error_message;
};

class Client {
public:
    explicit Client(Config const& config) : m_config(config) {}
    OrderResponse submit_order(Order const&);
    OrderResponse update_order(std::string const& order_id, double price);
    void retrieve_orders();
private:
    const Config m_config;
};

std::string get_current_timestamp_as_string();
std::string hex_hmac_sha384(const std::string& key, const std::string& data);

}
