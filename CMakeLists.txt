# At least CMake >= Version 3.16 is required
cmake_minimum_required(VERSION 3.16)

# Defining project name, version and used languages
project(CHIP-8 
VERSION 0.1.0 
LANGUAGES C CXX
)
# CMake Build options
option(CP8_BUILD_TESTS "Build the tests" OFF)
option(CP8_FETCH_SDL "Always download and build SDL2 instead of using an installed version" OFF)
set(CP8_SDL2_MIN_VERSION "2.0.0" CACHE STRING "Minimum SDL2 version required when using an installed SDL2")
set(CP8_SDL2_FETCH_TAG "release-2.32.10" CACHE STRING "SDL2 git tag that is downloaded if no installed SDL2 is used")

# These options only affect the build if the built-type is Debug - otherwise they will be ignored
option(CP8_DEBUG_PRINT_BYTECODE "Determines whether the instructions are printed" OFF)
option(CP8_DEBUG_TRACE_EXECUTION "Determines whether the execution shall be traced" OFF)

# C99 standard is required to build the emulator
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# C++ 14 needed for Google Test Framework
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CP8_BUILD_TESTS)
    # enabling tests
    enable_testing()
endif()

get_filename_component(SOURCE_PATH_FRONTEND ${PROJECT_SOURCE_DIR}/chip8/frontend/src ABSOLUTE)

 # Adding CheckIncludeFile CMake module
include(CheckIncludeFile)
# Adding own cmake scripts
include(./cmake/chip8_check.cmake)
include(./cmake/chip8_sdl.cmake)

# Checking dependencies
CHIP8_Check_Dependencies()

# Find SDL2 (or download it if it is not installed)
CHIP8_Find_SDL2()

# Build CHIP-8 project
add_subdirectory(chip8)
