cmake_minimum_required(VERSION 3.5)
project(main)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

find_package(yaml-cpp REQUIRED)
if(NOT yaml-cpp_FOUND)
    message(FATAL_ERROR "yaml-cpp not found!")
else()
    message("yaml-cpp found!")
endif()


set(SOURCE_FILES main.cpp)
add_executable(main ${SOURCE_FILES})

target_include_directories(main PUBLIC ${ZeroMQ_INCLUDE_DIR} ${YAML_CPP_INCLUDE_DIR})
target_link_libraries(main PRIVATE yaml-cpp::yaml-cpp ${ZeroMQ_LIBRARY})
#target_link_libraries(main ${ZeroMQ_LIBRARY} yaml-cpp)
