cmake_minimum_required(VERSION 2.6)
project(lsh)

add_executable(lsh main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

 

install(TARGETS lsh RUNTIME DESTINATION bin)
