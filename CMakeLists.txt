cmake_minimum_required(VERSION 3.5)

project(simu LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
FILE(GLOB SIM_PATH *.cpp)
add_executable(simulator ${SIM_PATH})