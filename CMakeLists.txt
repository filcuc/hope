cmake_minimum_required(VERSION 3.2)

include(cmake/gtest_downloader.cmake)

project(HopeProject)

# Add Test option
option(ENABLE_TEST "Enable test" OFF)

# Add Coverage option
option(ENABLE_COVERAGE "Enable coverage" OFF)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)

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

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(lib)
add_subdirectory(examples)
if (ENABLE_TEST)
  download_and_build_gtest()
  add_subdirectory(tests)
endif()
