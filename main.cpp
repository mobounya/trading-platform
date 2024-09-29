#include <Bitfinex/Client.h>
#include <Bitfinex/ENUMS.h>
#include <boost/program_options.hpp>
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
    client.submit_order(order);
}

int main(int argc, char **argv)
{
    boost::program_options::options_description general_options("Supported general options");
    general_options.add_options()("help", "print help message");
    general_options.add_options()("order", "Place a new order");

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

        if (variables_map.count("order")) {
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
