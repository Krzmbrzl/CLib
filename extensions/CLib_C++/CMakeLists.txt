cmake_minimum_required(VERSION 3.0)

project(clib)

add_executable(clib main.cpp Utils.cpp ArmaRequest.cpp)

install(TARGETS clib RUNTIME DESTINATION bin)
