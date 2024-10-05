#include <Bitfinex/Client.h>
#include <cpr/api.h>
#include <iostream>
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Bitfinex {

OrderResponse Client::submit_order(Order const& order)
{
    const std::string endpoint = "/v2/auth/w/order/submit";
    const std::string current_timestamp = get_current_timestamp_as_string();

    // Positive means buy, negative means sell
    const double amount = (order.side == OrderSide::SELL) ? (order.amount * -1) : order.amount;

    const std::string body = std::format(R"({{ "symbol": "{}", "type": "{}", "amount": "{}", "price": "{}" }})", order.symbol, order_type_to_string(order.type), amount, order.price);
    const std::string signature = hex_hmac_sha384(this->m_config.SECRET_KEY, "/api" + endpoint + current_timestamp + body);
    cpr::Response res = cpr::Post(cpr::Url { this->m_config.BASE_ENDPOINT + endpoint }, cpr::Body { body }, cpr::Header { { "Content-type", "application/json" }, {"accept", "application/json"}, { "bfx-nonce", current_timestamp },
                                    { "bfx-apikey", this->m_config.API_KEY }, { "bfx-signature", signature } });

    OrderResponse order_response;
    order_response.http_status = res.status_code;
    if (order_response.http_status != 200)
        return order_response;
    json json_response = json::parse(res.text);
    order_response.message = json_response[6];
    if (order_response.message != "SUCCESS")
        return order_response;
    const double order_amount = json_response[4][0][6];
    order_response.order_id = json_response[4][0][0];
    order_response.symbol = json_response[4][0][3];
    order_response.amount = (order_amount < 0) ? (order_amount * -1) : order_amount;
    order_response.side = (order_amount < 0) ? OrderSide::SELL : OrderSide::BUY;
    order_response.type = json_response[4][0][8];
    order_response.price = json_response[4][0][16];
    return order_response;
}

OrderResponse Client::update_order(std::string const& order_id, double price)
{
    const std::string endpoint = "/v2/auth/w/order/update";
    const std::string current_timestamp = get_current_timestamp_as_string();
    const std::string body = std::format(R"({{ "id": "{}", "price": "{}" }})", order_id, price);
    const std::string signature = hex_hmac_sha384(this->m_config.SECRET_KEY, "/api" + endpoint + current_timestamp + body);
    cpr::Response response = cpr::Post(cpr::Url { this->m_config.BASE_ENDPOINT + endpoint }, cpr::Body { body }, cpr::Header { { "Content-type", "application/json" }, {"accept", "application/json"}, { "bfx-nonce", current_timestamp },
                                                                                                        { "bfx-apikey", this->m_config.API_KEY }, { "bfx-signature", signature } });

    OrderResponse order_response;
    order_response.http_status = response.status_code;
    if (order_response.http_status != 200)
        return order_response;
    json json_response = json::parse(response.text);
    order_response.message = json_response[6];
    if (order_response.message != "SUCCESS")
        return order_response;
    const double order_amount = json_response[4][6];
    order_response.order_id = json_response[4][0];
    order_response.symbol = json_response[4][3];
    order_response.amount = (order_amount < 0) ? (order_amount * -1) : order_amount;
    order_response.side = (order_amount < 0) ? OrderSide::SELL : OrderSide::BUY;
    order_response.type = json_response[4][8];
    order_response.price = json_response[4][16];
    return order_response;
}

OrderResponse Client::cancel_order(std::string const& order_id)
{
    const std::string endpoint = "/v2/auth/w/order/cancel";
    const std::string current_timestamp = get_current_timestamp_as_string();
    const std::string body = std::format(R"({{ "id": {} }})", order_id);
    const std::string signature = hex_hmac_sha384(this->m_config.SECRET_KEY, "/api" + endpoint + current_timestamp + body);

    cpr::Response response = cpr::Post(cpr::Url { this->m_config.BASE_ENDPOINT + endpoint }, cpr::Body { body }, cpr::Header { { "Content-type", "application/json" }, {"accept", "application/json"}, { "bfx-nonce", current_timestamp },
                                                                                                                     { "bfx-apikey", this->m_config.API_KEY }, { "bfx-signature", signature } });

    OrderResponse order_response;
    order_response.http_status = response.status_code;
    if (order_response.http_status != 200)
        return order_response;
    json json_response = json::parse(response.text);
    order_response.message = json_response[6];
    order_response.order_id = json_response[4][0];
    return order_response;
}

TickerResponse Client::get_ticker(std::string const& symbol)
{
    const std::string url = "https://api-pub.bitfinex.com";
    const std::string endpoint = "/v2/ticker/" + symbol;
    cpr::Response response = cpr::Get(cpr::Url { url + endpoint });

    TickerResponse ticker_response;
    ticker_response.http_status = response.status_code;
    if (ticker_response.http_status != 200)
        return ticker_response;
    json json_response = json::parse(response.text);
    ticker_response.bid = json_response[0];
    ticker_response.last_price = json_response[6];
    ticker_response.volume = json_response[7];
    return ticker_response;
}

std::optional<OrderBook> Client::retrieve_orders(std::string const& symbol)
{
    const std::string endpoint = "/v2/auth/r/orders/" + symbol;
    const std::string current_timestamp = get_current_timestamp_as_string();
    const std::string signature = hex_hmac_sha384(this->m_config.SECRET_KEY, "/api" + endpoint + current_timestamp);

    cpr::Response response = cpr::Post(cpr::Url { this->m_config.BASE_ENDPOINT + endpoint }, cpr::Body {}, cpr::Header { { "Content-type", "application/json" }, {"accept", "application/json"}, { "bfx-nonce", current_timestamp },
                                            { "bfx-apikey", this->m_config.API_KEY }, { "bfx-signature", signature } });
    if (response.status_code != 200)
        return {};
    json json_response = json::parse(response.text);
    if (json_response.empty())
        return OrderBook {};
    OrderBook order_book;
    std::for_each(json_response.cbegin(), json_response.cend(), [&order_book](json const& order_element) {
        double order_amount = order_element[6];
        // Positive means buy, negative means sell
        order_amount = (order_amount < 0) ? order_amount * -1 : order_amount;
        OrderSide side = (order_element[6] < 0) ? OrderSide::SELL : OrderSide::BUY;
        ulong order_id = order_element[0];
        Order order { .order_id = std::to_string(order_id), .side = side, .symbol = order_element[3], .amount = order_amount,
                        .type = order_type_from_string(order_element[8]), .price = order_element[16], .creation_date = unix_to_iso_utc(order_element[4]) };
        order_book.append_order(order);
    });
    return order_book;
}

IncreasePositionResponse Client::increase_position(PositionSide side, std::string const& symbol, double amount)
{
    const std::string endpoint = "/v2/auth/w/position/increase";
    const std::string current_timestamp = get_current_timestamp_as_string();
    // (positive for long, negative for short)
    amount = (side == PositionSide::SHORT) ? (amount * -1) : amount;
    const std::string body = std::format(R"({{ "symbol": "{}", "amount": "{}" }})", symbol, amount);
    const std::string signature = hex_hmac_sha384(this->m_config.SECRET_KEY, "/api" + endpoint + current_timestamp + body);

    std::cout << body << std::endl;
    cpr::Response response = cpr::Post(cpr::Url { this->m_config.BASE_ENDPOINT + endpoint }, cpr::Body{ body }, cpr::Header { { "Content-type", "application/json" }, {"accept", "application/json"}, { "bfx-nonce", current_timestamp },
                                                                                                 { "bfx-apikey", this->m_config.API_KEY }, { "bfx-signature", signature } });
    IncreasePositionResponse position_response;
    position_response.http_status = response.status_code;
    if (position_response.http_status != 200)
        return position_response;
    json json_response = json::parse(response.text);
    position_response.message = json_response[6];
    return position_response;
}

std::string get_current_timestamp_as_string()
{
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    return std::to_string(now_ms.time_since_epoch().count());
}

std::string unix_to_iso_utc(long long milliseconds_unix_time_stamp)
{
    // Convert Unix timestamp from milliseconds to seconds and remaining milliseconds
    long long seconds = milliseconds_unix_time_stamp / 1000;
    long long milliseconds = milliseconds_unix_time_stamp % 1000;

    // Convert to time_t
    std::time_t time = static_cast<time_t>(seconds);

    // Convert to tm struct
    std::tm* tmUTC = std::gmtime(&time);

    // Create an ostringstream for formatting
    std::ostringstream oss;

    // Format the time as ISO 8601 UTC
    oss << std::put_time(tmUTC, "%Y-%m-%dT%H:%M:%S");

    // Always add milliseconds
    oss << '.' << std::setfill('0') << std::setw(3) << milliseconds;

    oss << 'Z';  // UTC designator

    return oss.str();
}

// https://www.okx.com/docs-v5/en/#overview-rest-authentication-signature
std::string hex_hmac_sha384(std::string const& key, std::string const& data)
{
    unsigned char hash[EVP_MAX_MD_SIZE];
    size_t hash_len;

    EVP_MAC* mac = EVP_MAC_fetch(nullptr, "HMAC", nullptr);
    if (!mac) {
        throw std::runtime_error("Failed to fetch HMAC");
    }

    EVP_MAC_CTX* ctx = EVP_MAC_CTX_new(mac);
    if (!ctx) {
        EVP_MAC_free(mac);
        throw std::runtime_error("Failed to create MAC context");
    }

    OSSL_PARAM params[] = {
        OSSL_PARAM_construct_utf8_string("digest", const_cast<char*>("SHA384"), 0),
        OSSL_PARAM_construct_end()
    };

    if (EVP_MAC_init(ctx, reinterpret_cast<unsigned char const*>(key.data()), key.size(), params) != 1) {
        EVP_MAC_CTX_free(ctx);
        EVP_MAC_free(mac);
        throw std::runtime_error("Failed to initialize MAC");
    }

    if (EVP_MAC_update(ctx, reinterpret_cast<unsigned char const*>(data.data()), data.size()) != 1) {
        EVP_MAC_CTX_free(ctx);
        EVP_MAC_free(mac);
        throw std::runtime_error("Failed to update MAC");
    }

    if (EVP_MAC_final(ctx, hash, &hash_len, sizeof(hash)) != 1) {
        EVP_MAC_CTX_free(ctx);
        EVP_MAC_free(mac);
        throw std::runtime_error("Failed to finalize MAC");
    }

    EVP_MAC_CTX_free(ctx);
    EVP_MAC_free(mac);

    std::stringstream ss;

    for (unsigned int i = 0; i < hash_len; i++)
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);

    return ss.str();
}

std::ostream& operator<<(std::ostream& cin, Order const& order)
{
    cin << "{ order id: " << order.order_id << ", side: " << order_side_to_string(order.side) << ", symbol: "
        << order.symbol << ", amount: " << order.amount << ", type: " << order_type_to_string(order.type) <<
        ", price: " << order.price << ", creation date: " << order.creation_date << " }";
    return cin;
}

}
