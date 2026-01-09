cmake_minimum_required(VERSION 3.28)

include(../cmake/add_day.cmake)

set(CURRENT_DAY 21)

project(advent_of_code_2023)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

generate_to_current_day()
