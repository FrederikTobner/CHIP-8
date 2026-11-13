cmake_minimum_required(VERSION 3.16)

project(CHIP-8 VERSION 0.1.0 LANGUAGES C CXX)

# CMake Build options
option(BUILD_TESTING "Build the tests" OFF)
option(CP8_FETCH_SDL "Always download and build SDL2 instead of using an installed version" OFF)
set(CP8_SDL2_MIN_VERSION "2.0.0" CACHE STRING "Minimum SDL2 version required when using an installed SDL2")
set(CP8_SDL2_FETCH_TAG "release-2.32.10" CACHE STRING "SDL2 git tag that is downloaded if no installed SDL2 is used")

# These options only affect the build if the built-type is Debug - otherwise they will be ignored
option(CP8_DEBUG_PRINT_BYTECODE "Determines whether the instructions are printed" OFF)
option(CP8_DEBUG_TRACE_EXECUTION "Determines whether the execution shall be traced" OFF)

# Adding CheckIncludeFile CMake module
include(CheckIncludeFile)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# C99 standard is required to build the emulator
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# C++ 14 needed for Google Test Framework
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(BUILD_TESTING)
    # enabling tests
    enable_testing()
endif()

# Adding own cmake scripts
include(chip8_check)
include(chip8_sdl)

# Checking dependencies
CHIP8_Check_Dependencies()

# Find SDL2 (or download it if it is not installed)
CHIP8_Find_SDL2()

# Build CHIP-8 project
add_subdirectory(chip8)
