#include <Bitfinex/Client.h>
#include <cpr/api.h>
#include <iostream>
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <string>

namespace Bitfinex {

void Client::submit_order(Order const& order)
{
    const std::string endpoint = "/v2/auth/w/order/submit";
    const std::string current_timestamp = get_current_timestamp_as_string();
    // Positive means buy, negative means sell
    double amount = (order.side == OrderSide::SELL) ? (order.amount * -1) : order.amount;

    const std::string body = std::format(R"({{ "symbol": "{}", "type": "{}", "amount": "{}", "price": "{}" }})", order.symbol, order_type_to_string(order.type), amount, order.price);
    const std::string signature = hex_hmac_sha384(this->m_config.SECRET_KEY, "/api" + endpoint + current_timestamp + body);
    cpr::Response res = cpr::Post(cpr::Url { this->m_config.BASE_ENDPOINT + endpoint }, cpr::Body { body }, cpr::Header { { "Content-type", "application/json" }, {"accept", "application/json"}, { "bfx-nonce", current_timestamp },
                                                                                                        { "bfx-apikey", this->m_config.API_KEY }, { "bfx-signature", signature } });
    std::cout << res.status_code << std::endl;
    std::cout << res.text << std::endl;
}

void Client::retrieve_orders()
{
    const std::string endpoint = "/v2/auth/r/wallets";
    const std::string current_timestamp = get_current_timestamp_as_string();
    const std::string signature = hex_hmac_sha384(this->m_config.SECRET_KEY, "/api" + endpoint + current_timestamp);

    cpr::Response r = cpr::Post(cpr::Url { this->m_config.BASE_ENDPOINT + endpoint }, cpr::Body {}, cpr::Header { { "Content-type", "application/json" }, {"accept", "application/json"}, { "bfx-nonce", current_timestamp },
                                            { "bfx-apikey", this->m_config.API_KEY }, { "bfx-signature", signature } });

    std::cout << r.status_code << std::endl;
    std::cout << r.text << std::endl;
}

std::string get_current_timestamp_as_string()
{
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    return std::to_string(now_ms.time_since_epoch().count());
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

}
