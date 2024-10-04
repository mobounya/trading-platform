cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 20)
project(trader)

set(nlohmann_json_DIR "/home/linuxbrew/.linuxbrew/Cellar/nlohmann-json/3.11.3/share/cmake/nlohmann_json")

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 3b15fa82ea74739b574d705fea44959b58142eb8) # Replace with your desired git commit from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

find_package(Boost REQUIRED COMPONENTS program_options)
find_package(OpenSSL REQUIRED COMPONENTS Crypto)
find_package(nlohmann_json REQUIRED) # https://github.com/nlohmann/json

add_executable(trader main.cpp
        Bitfinex/Client.cpp
        Bitfinex/Client.h
        Bitfinex/ENUMS.h
        Bitfinex/ENUMS.cpp
        Bitfinex/OrderBook.h
        Bitfinex/OrderBook.cpp
        Bitfinex/Forward.h)

target_link_libraries(trader PRIVATE Boost::program_options)
target_link_libraries(trader PRIVATE cpr::cpr)
target_link_libraries(trader PRIVATE OpenSSL::Crypto)
target_link_libraries(trader PRIVATE nlohmann_json::nlohmann_json)

target_include_directories(trader PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(trader PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(trader PRIVATE ${OPENSSL_INCLUDE_DIR})
target_include_directories(trader PRIVATE ${INTERFACE_INCLUDE_DIRECTORIES})
