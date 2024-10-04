#include <Bitfinex/Client.h>
#include <Bitfinex/ENUMS.h>
#include <boost/algorithm/string/trim.hpp>
#include <boost/program_options.hpp>
#include <format>
#include <iostream>

class SideValue : public boost::program_options::typed_value<std::string>
{
public:
    SideValue(std::string* store_to) : boost::program_options::typed_value<std::string>(store_to) {}

    virtual void xparse(boost::any& v, const std::vector<std::string>& values) const override
    {
        boost::program_options::validators::check_first_occurrence(v);
        const std::string& side = boost::program_options::validators::get_single_string(values);
        if (!Bitfinex::is_valid_order_side(side))
            throw boost::program_options::validation_error(boost::program_options::validation_error::invalid_option_value, "side");

        v = boost::any(Bitfinex::order_side_from_string(side));
    }
};

class TypeValue : public boost::program_options::typed_value<std::string>
{
public:
    TypeValue(std::string* store_to) : boost::program_options::typed_value<std::string>(store_to) {}

    virtual void xparse(boost::any& v, const std::vector<std::string>& values) const override
    {
        boost::program_options::validators::check_first_occurrence(v);
        const std::string& type = boost::program_options::validators::get_single_string(values);
        if (!Bitfinex::is_valid_order_type(type))
            throw boost::program_options::validation_error(boost::program_options::validation_error::invalid_option_value, "type");
        if (type != "exchange_limit") {
            std::cout << "oops ! Only exchange limit orders are supported at this time" << std::endl;
            std::exit(0);
        }
        v = boost::any(Bitfinex::order_type_from_string(type));
    }
};

double read_positive_double_from_cli()
{
    std::string user_input;
    while (true) {
        std::getline(std::cin, user_input);
        try {
            auto value = boost::lexical_cast<double>(user_input);
            if (value <= 0)
                std::cerr << "Please enter a positive number:" << std::endl;
            else
                return value;
        } catch (const std::exception &ignore) {
            std::cerr << "Please enter a positive number:" << std::endl;
        }
    }
}

std::string read_string_from_cli(std::vector<std::string> const &allowed_values)
{
    std::string user_input;
    while (true) {
        std::getline(std::cin, user_input);
        boost::trim(user_input);
        if (std::count(allowed_values.begin(), allowed_values.end(), user_input) > 0)
            break;
    }
    return user_input;
}

void execute_place_order_command(boost::program_options::variables_map const& variables_map)
{
    if (variables_map.count("side") == 0)
        throw boost::program_options::required_option("side");
    if (variables_map.count("symbol") == 0)
        throw boost::program_options::required_option("symbol");
    if (variables_map.count("amount") == 0)
        throw boost::program_options::required_option("amount");
    if (variables_map.count("type") == 0)
        throw boost::program_options::required_option("type");
    if (variables_map.count("price") == 0)
        throw boost::program_options::required_option("price");

    Bitfinex::Config config { .BASE_ENDPOINT = dotenv::getenv("BASE_ENDPOINT"), .API_KEY = dotenv::getenv("API_KEY"),
            .SECRET_KEY = dotenv::getenv("SECRET_KEY") };

    Bitfinex::Order order { .side = variables_map["side"].as<Bitfinex::OrderSide>(), .symbol = variables_map["symbol"].as<std::string>(),
                                .amount = variables_map["amount"].as<double>(), .type = variables_map["type"].as<Bitfinex::OrderType>(),
                                    .price = variables_map["price"].as<double>()};

    Bitfinex::Client client(config);
    Bitfinex::OrderResponse order_response = client.submit_order(order);
    if (order_response.http_status != 200) {
        std::cerr << "An error occurred, please try again later" << std::endl;
        exit(1);
    }
    if (order_response.message != "SUCCESS") {
        std::cerr << "Oops ! order didn't go through" << std::endl;
        exit(2);
    }

    std::cout << std::format(R"(Placed a {} {} order for pair {} for the price of {} for {} amount, order id: ({}))", order_response.type, order_side_to_string(order_response.side), order_response.symbol
                                , order_response.price, order_response.amount, order_response.order_id) << std::endl;

    std::cout << "Would you like to change the order price ? (y,n) " << std::endl;
    if (read_string_from_cli({"yes", "y", "no", "n"})[0] == 'y') {
        std::cout << "Please enter the new order price:" << std::endl;
        double new_price = read_positive_double_from_cli();
        order_response = client.update_order(std::to_string(order_response.order_id), new_price);

        if (order_response.http_status != 200) {
            std::cerr << "An error occurred, please try again later" << std::endl;
            exit(1);
        }
        if (order_response.message != "SUCCESS")
            std::cerr << "Oops ! could not update the price" << std::endl;
        else
            std::cout << "Successfully changed order price to " << order_response.price << std::endl;
    }

    std::cout << "Would you like to cancel the order (" << order_response.order_id << ") ? (y,n)" << std::endl;
    if (read_string_from_cli({"yes", "y", "no", "n"})[0] == 'y') {
        order_response = client.cancel_order(std::to_string(order_response.order_id));

        if (order_response.http_status != 200) {
            std::cerr << "An error occurred, please try again later" << std::endl;
            exit(1);
        }
        if (order_response.message != "SUCCESS") {
            std::cerr << "Oops ! could not cancel order" << std::endl;
            exit(2);
        }

        std::cout << "Successfully submitted order (" << order_response.order_id << ") for cancellation" << std::endl;
    }
}

void execute_get_ticker_info_command(std::string const& symbol)
{
    Bitfinex::TickerResponse ticker_response = Bitfinex::Client::get_ticker(symbol);
    if (ticker_response.http_status != 200) {
        std::cerr << "An error occurred, please try again later" << std::endl;
        exit(1);
    }
    std::cout << "Price of the last trade: " << ticker_response.last_price << std::endl;
    std::cout << "Price of last highest bid: " << ticker_response.bid << std::endl;
    std::cout << "Daily volume: " << ticker_response.volume << std::endl;
}

int main(int argc, char **argv)
{
    boost::program_options::options_description general_options("Supported general options");
    general_options.add_options()("help", "print help message");
    general_options.add_options()("order", "Place a new order");
    general_options.add_options()("ticker", boost::program_options::value<std::string>(), "Print information about the given ticker");

    boost::program_options::options_description order_options("Supported order options");
    order_options.add_options()("side", new SideValue(nullptr), "Order side [BUY, SELL]");
    order_options.add_options()("symbol", boost::program_options::value<std::string>(), "The trading pair symbol to submit the order on.");
    order_options.add_options()("amount", boost::program_options::value<double>()->notifier([](double value) {
        if (value <= 0)
            throw boost::program_options::validation_error(boost::program_options::validation_error::invalid_option_value, "amount");
    }), "Amount of order");
    order_options.add_options()("type", new TypeValue(nullptr), "The type of the order");
    order_options.add_options()("price", boost::program_options::value<double>()->notifier([](double value) {
        if (value <= 0)
            throw boost::program_options::validation_error(boost::program_options::validation_error::invalid_option_value, "price");
    }), "Price of the order");

    boost::program_options::options_description all_options("all options");
    all_options.add(general_options).add(order_options);

    try {
        boost::program_options::variables_map variables_map;
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, all_options), variables_map);
        boost::program_options::notify(variables_map);

        if (variables_map.count("ticker")) {
            std::string symbol = variables_map["ticker"].as<std::string>();
            execute_get_ticker_info_command(symbol);
        } else if (variables_map.count("order")) {
            dotenv::init(".env");

            if (dotenv::getenv("BASE_ENDPOINT").empty()) {
                std::cerr << "Please set BASE_ENDPOINT in the .env file" << std::endl;
                return 1;
            }
            if (dotenv::getenv("API_KEY").empty()) {
                std::cerr << "Please set API_KEY in the .env file" << std::endl;
                return 1;
            }
            if (dotenv::getenv("SECRET_KEY").empty()) {
                std::cerr << "Please set SECRET_KEY in the .env file" << std::endl;
                return 1;
            }

            execute_place_order_command(variables_map);
        } else
            std::cout << all_options << std::endl;
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }
    return 0;
}
