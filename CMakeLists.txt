cmake_minimum_required(VERSION 3.28)
project(trader)

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 3b15fa82ea74739b574d705fea44959b58142eb8) # Replace with your desired git commit from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

find_package(Boost REQUIRED COMPONENTS program_options)

add_executable(trader main.cpp)

target_link_libraries(trader PRIVATE Boost::program_options)
target_link_libraries(trader PRIVATE cpr::cpr)

target_include_directories(trader PRIVATE ${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 20)
