cmake_minimum_required(VERSION 3.28)

project(adventofcode-root LANGUAGES CXX)

# Sensible defaults; per-year CMake files may override these if needed.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Discover year subdirectories that contain their own CMakeLists.txt
file(GLOB YEAR_CMAKES RELATIVE ${CMAKE_SOURCE_DIR} "20*/CMakeLists.txt")
if(YEAR_CMAKES)
  foreach(_cm ${YEAR_CMAKES})
    get_filename_component(_dir ${_cm} DIRECTORY)
    message(STATUS "Adding year directory: ${_dir}")
    # When building from the repo root, prefix per-year targets to avoid
    # collisions between identically-named day targets from different years.
    set(AOC_TARGET_PREFIX "${_dir}_")
    add_subdirectory(${_dir})
    unset(AOC_TARGET_PREFIX)
  endforeach()
else()
  message(WARNING "No year directories with CMakeLists.txt found (pattern 20*/CMakeLists.txt).")
endif()

# Note: building from the repository root after running CMake will build all
# targets registered by each year's CMakeLists. Individual year folders can
# still be configured and built independently (e.g. `cmake -S 2023 -B 2023/build`).
