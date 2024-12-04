cmake_minimum_required(VERSION 3.29)

set(CURRENT_DAY 4)

project(advent_of_code_2024)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

function (add_day DAY)
  add_executable(day${DAY} src/day${DAY}.cpp)
  target_include_directories(day${DAY} PRIVATE ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../common)
  set_source_files_properties(src/day${DAY}.cpp PROPERTIES COMPILE_FLAGS "-DCOMPILE_UTILS=1")
  add_custom_target(
    run-day${DAY}
    COMMAND $<TARGET_FILE:day${DAY}> ${CMAKE_CURRENT_SOURCE_DIR}/input/day${DAY}.txt
    DEPENDS day${DAY}
  )
  add_custom_target(
    run-day${DAY}-example
    COMMAND $<TARGET_FILE:day${DAY}> ${CMAKE_CURRENT_SOURCE_DIR}/input/day${DAY}exemple.txt
    DEPENDS day${DAY}
  )

  if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/day${DAY}bis.cpp)
    add_executable(day${DAY}bis src/day${DAY}bis.cpp)
    target_include_directories(day${DAY}bis PRIVATE ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../common)
    set_source_files_properties(src/day${DAY}bis.cpp PROPERTIES COMPILE_FLAGS "-DCOMPILE_UTILS=1")
    add_custom_target(
      run-day${DAY}bis
      COMMAND $<TARGET_FILE:day${DAY}bis> ${CMAKE_CURRENT_SOURCE_DIR}/input/day${DAY}.txt
      DEPENDS day${DAY}
    )
    add_custom_target(
      run-day${DAY}bis-example
      COMMAND $<TARGET_FILE:day${DAY}bis> ${CMAKE_CURRENT_SOURCE_DIR}/input/day${DAY}exemple.txt
      DEPENDS day${DAY}
    )
  endif()
endfunction()

foreach (DAY RANGE 1 ${CURRENT_DAY})
  add_day(${DAY})
endforeach()
