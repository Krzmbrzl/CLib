cmake_minimum_required(VERSION 3.0)

project(clib)

option(Build64 "Whether to build the 64bit binaries instead of the 32bit ones" OFF)

set(SHARED_SOURCES Utils.cpp)

add_executable(main main.cpp)
add_library(clib SHARED CLibEntry.cpp ArmaExtension.cpp ArmaRequest.cpp ${SHARED_SOURCES})

set_target_properties(clib PROPERTIES PUBLIC_HEADER DllEntry.hpp)

if(Build64)
    add_definitions(-DCLIB64)

    # set 64 bit names
    set_target_properties(clib PROPERTIES PREFIX "" OUTPUT_NAME "clib_x64")
else(Build64)
    add_definitions(-DCLIB32)

    # set 32bit names
    set_target_properties(clib PROPERTIES PREFIX "" OUTPUT_NAME "clib")
endif(Build64)

add_definitions(-DCLIB_VERSION="2.0.0")


if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    message("Detected Linux system - building linux binaries...")

    add_definitions(-DLINUX)

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -ldl")

    if(Build64)
        message("Building 64bit binaries")
        set_target_properties(main PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
        set_target_properties(clib PROPERTIES COMPILE_FLAGS "-m64" LINK_FLAGS "-m64")
        target_link_libraries(main /usr/lib/x86_64-linux-gnu/libdl.so)
        target_link_libraries(clib /usr/lib/x86_64-linux-gnu/libpthread.so)
    else(Build64)
        message("Building 32bit binaries")
        set_target_properties(main PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
        set_target_properties(clib PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
        target_link_libraries(main /usr/lib32/libdl.so)
        target_link_libraries(clib /usr/lib32/libpthread.so)
    endif(Build64)
    
endif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    message("Detected Windows system - building Windows binaries...")

    add_definitions(-DWINDOWS)

    if(Build64)
        message("Building 64bit binaries")
    else(Build64)
        message("Building 32bit binaries")
    endif(Build64)
endif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")