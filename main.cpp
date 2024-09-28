#include <iostream>
#include <boost/program_options.hpp>

int main(int argc, char **argv) {
    boost::program_options::options_description desc("Supported COMMANDs");
    desc.add_options()("help", "Print help");
    desc.add_options()("place-new-order", "Place a new order");

    boost::program_options::variables_map variables_map;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), variables_map);
    boost::program_options::notify(variables_map);

    if (variables_map.count("help")) {
        std::cout << desc << std::endl;
    } else if (variables_map.count("place-new-order")) {
        std::cerr << "FIXME: implement place-new-order" << std::endl;
        return 1;
    }
    return 0;
}
