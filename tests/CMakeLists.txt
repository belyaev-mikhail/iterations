# cmake_minimum_required(VERSION 3.1)
project(iterations_tests)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

include_directories(..)
include_directories(src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

file(GLOB cpps ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(iterations_tests run_tests.cpp ${cpps})
target_link_libraries(iterations_tests ${GTEST_BOTH_LIBRARIES})
