cmake_minimum_required(VERSION 3.15)
project(Nerve)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lrt -lpthread -lm -lboost_system")

include_directories(/usr/include/openssl/)
include_directories(/usr/include/cpprest/)
link_libraries(crypto)
link_libraries(cpprest)

add_executable(Nerve main.cpp Brain.cpp Brain.h BrainChain.cpp BrainChain.h HttpHandler.cpp HttpHandler.h)
