cmake_minimum_required(VERSION 3.5)
project(main)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CheckCXXSourceCompiles)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}  "-g -std=c++20 -Wall")

find_package(PkgConfig)
pkg_check_modules(PC_ZeroMQ QUIET zmq)

find_path(ZeroMQ_INCLUDE_DIR
        NAMES zmq.hpp
        PATHS ${PC_ZeroMQ_INCLUDE_DIRS}
        )

find_library(ZeroMQ_LIBRARY
        NAMES zmq
        PATHS ${PC_ZeroMQ_LIBRARY_DIRS}
        )

set(SOURCE_FILES main.cpp)
add_executable(main ${SOURCE_FILES})

target_include_directories(main PUBLIC ${ZeroMQ_INCLUDE_DIR})
target_link_libraries(main PUBLIC ${ZeroMQ_LIBRARY})
