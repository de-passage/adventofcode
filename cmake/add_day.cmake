##
# add_day_target
#
# Create a build target and run targets for a single puzzle day.
# Parameters:
#  - DAY: logical target name (e.g. `day12`). The function will create an
#    executable target named `${DAY}` by compiling `src/${DAY}.cpp`.
#  - INPUT: base name of the real input file (without `.txt`) located in
#    `input/` (e.g. `day12`). The generated run target depends on
#    `input/${INPUT}.txt`.
#  - EXAMPLE_INPUT: base name of the example input file (without `.txt`).
#  - DAY_NB: numeric day index used when fetching the input (passed to the
#    `scripts/get_input.bash` helper).
#
# Side effects:
#  - adds executable target `${DAY}` (sources: `src/${DAY}.cpp`)
#  - sets include directories to `../common` for the target
#  - applies `-DCOMPILE_UTILS=1` compile flag to the source file
#  - registers a custom command that fetches `input/${INPUT}.txt` when missing
#  - adds `run-${DAY}` and `run-${DAY}-example` custom targets that invoke
#    the built executable with real/example inputs respectively
function(add_day_target DAY INPUT EXAMPLE_INPUT DAY_NB)
  # Allow an optional prefix to be supplied by the top-level build so targets
  # become unique when multiple years are added into the same build.
  if(DEFINED AOC_TARGET_PREFIX)
    set(_PREFIX ${AOC_TARGET_PREFIX})
  else()
    set(_PREFIX "")
  endif()

  set(_TARGET "${_PREFIX}${DAY}")
  add_executable(${_TARGET} ${CMAKE_CURRENT_LIST_DIR}/src/${DAY}.cpp)
  target_include_directories(${_TARGET} PRIVATE ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../common)
  set_source_files_properties(src/${DAY}.cpp PROPERTIES COMPILE_FLAGS "-DCOMPILE_UTILS=1")

  set(_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/input/${INPUT}.txt)
  set(_EXAMPLE_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/input/${EXAMPLE_INPUT}.txt)

  # Determine the year directory name (e.g. "2023") from the current
  # CMake source directory so the input fetch helper receives the correct
  # year instead of a hardcoded value.
  get_filename_component(_YEAR ${CMAKE_CURRENT_SOURCE_DIR} NAME)

  add_custom_command(
    OUTPUT ${_INPUT}
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/../scripts/get_input.bash" ${_YEAR} ${DAY_NB} ${_INPUT}
  )

  add_custom_target(
    run-${_TARGET}
    COMMAND $<TARGET_FILE:${_TARGET}> ${_INPUT}
    DEPENDS ${_TARGET} ${_INPUT}
  )
  add_custom_target(
    run-${_TARGET}-example
    COMMAND $<TARGET_FILE:${_TARGET}> ${_EXAMPLE_INPUT}
    DEPENDS ${_TARGET} ${_EXAMPLE_INPUT}
  )
endfunction()

##
# add_day
#
# Convenience wrapper that registers the canonical targets for a numbered day.
# Parameters:
#  - DAY: numeric day index (e.g. `12`). This macro will call
#    `add_day_target` to create `day<DAY>` and, if present, `day<DAY>bis`.
#
# Behavior:
#  - creates `day<N>` target using `src/day<N>.cpp` and `input/day<N>.txt`.
#  - if `src/day<N>bis.cpp` exists, also creates `day<N>bis`.
function (add_day DAY)
  add_day_target(day${DAY} day${DAY} day${DAY}exemple ${DAY})

  if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/day${DAY}bis.cpp)
    add_day_target(day${DAY}bis day${DAY} day${DAY}exemple ${DAY})
  endif()
endfunction()

##
# generate_to_current_day
#
# Macro that expands `add_day()` for every day from 1 to `${CURRENT_DAY}`.
# Use by setting `CURRENT_DAY` in the year `CMakeLists.txt`, then calling this
# macro to register all day targets in one call.
macro (generate_to_current_day)
foreach (DAY RANGE 1 ${CURRENT_DAY})
  add_day(${DAY})
endforeach()
endmacro()

