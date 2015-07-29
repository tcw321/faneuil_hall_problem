cmake_minimum_required(VERSION 3.2)
project(faneuil_hall_problem)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES state_machine.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/Users/tcw321/ClionProjects/faneuil_hall_problem")
add_executable(faneuil_hall_problem ${SOURCE_FILES})