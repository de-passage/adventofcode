cmake_minimum_required(VERSION 3.28)

set(CURRENT_DAY 20)

project(advent_of_code_2024)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

function(add_day_target DAY INPUT EXAMPLE_INPUT DAY_NB)
  add_executable(${DAY} src/${DAY}.cpp)
  target_include_directories(${DAY} PRIVATE ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../common)
  set_source_files_properties(src/${DAY}.cpp PROPERTIES COMPILE_FLAGS "-DCOMPILE_UTILS=1")

  set(_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/input/${INPUT}.txt)
  set(_EXAMPLE_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/input/${EXAMPLE_INPUT}.txt)

  add_custom_command(
    OUTPUT ${_INPUT}
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/get_input.bash" 2024 ${DAY_NB} ${_INPUT}
  )

  add_custom_target(
    run-${DAY}
    COMMAND $<TARGET_FILE:${DAY}> ${_INPUT}
    DEPENDS ${DAY} ${_INPUT}
  )
  add_custom_target(
    run-${DAY}-example
    COMMAND $<TARGET_FILE:${DAY}> ${_EXAMPLE_INPUT}
    DEPENDS ${DAY} ${_EXAMPLE_INPUT}
  )
endfunction()

function (add_day DAY)
  add_day_target(day${DAY} day${DAY} day${DAY}exemple ${DAY})

  if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/day${DAY}bis.cpp)
    add_day_target(day${DAY}bis day${DAY} day${DAY}exemple ${DAY})
  endif()
endfunction()

foreach (DAY RANGE 1 ${CURRENT_DAY})
  add_day(${DAY})
endforeach()

add_day_target(day6ter day6 day6exemple 6)
