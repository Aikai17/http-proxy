cmake_minimum_required(VERSION 3.14)
project(http_proxy)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS system)

add_executable(http_proxy main.cpp proxy_server.cpp)
target_include_directories(http_proxy PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(http_proxy PRIVATE Boost::system pthread)
