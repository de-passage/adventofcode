cmake_minimum_required(VERSION 3.29)

set(CURRENT_DAY 10)

project(advent_of_code_2024)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

function(add_day_target DAY INPUT EXAMPLE_INPUT)
  add_executable(${DAY} src/${DAY}.cpp)
  target_include_directories(${DAY} PRIVATE ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../common)
  set_source_files_properties(src/${DAY}.cpp PROPERTIES COMPILE_FLAGS "-DCOMPILE_UTILS=1")
  add_custom_target(
    run-${DAY}
    COMMAND $<TARGET_FILE:${DAY}> ${CMAKE_CURRENT_SOURCE_DIR}/input/${INPUT}.txt
    DEPENDS ${DAY}
  )
  add_custom_target(
    run-${DAY}-example
    COMMAND $<TARGET_FILE:${DAY}> ${CMAKE_CURRENT_SOURCE_DIR}/input/${EXAMPLE_INPUT}.txt
    DEPENDS ${DAY}
  )
endfunction()

function (add_day DAY)
  add_day_target(day${DAY} day${DAY} day${DAY}exemple)

  if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/day${DAY}bis.cpp)
    add_day_target(day${DAY}bis day${DAY} day${DAY}exemple)
  endif()
endfunction()

foreach (DAY RANGE 1 ${CURRENT_DAY})
  add_day(${DAY})
endforeach()

add_day_target(day6ter day6 day6exemple)
