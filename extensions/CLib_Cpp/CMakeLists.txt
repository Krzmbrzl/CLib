cmake_minimum_required(VERSION 3.0)

project(clib)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -ldl")

add_executable(clib main.cpp Utils.cpp ArmaRequest.cpp ArmaExtension.cpp)

target_link_libraries(clib /usr/lib/x86_64-linux-gnu/libdl.so)

install(TARGETS clib RUNTIME DESTINATION bin)