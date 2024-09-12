cmake_minimum_required(VERSION 3.0)
project(IRC_Bot)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS system)

add_executable(ircbot main.cpp)
target_link_libraries(ircbot Boost::system)
