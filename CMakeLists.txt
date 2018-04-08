cmake_minimum_required(VERSION 3.2)
project(HopeProject)

# Add Coverage option
option(ENABLE_COVERAGE "Enable coverage" OFF)

# Add strict warning checking for C++
if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
endif()

# Set compiler options when coverage is enabled
if (ENABLE_COVERAGE)
    message(STATUS "Enabling coverage")
    set(CMAKE_BUILD_TYPE Debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
endif()

add_subdirectory(lib)
add_subdirectory(examples)
