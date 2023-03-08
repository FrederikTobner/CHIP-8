# At least CMake >= Version 3.16 is required
cmake_minimum_required(VERSION 3.16)

# Defining project name, version and used languages
project(CHIP-8 
VERSION 0.1.0 
LANGUAGES C
)

# C99 standard is required to build the emulator
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# CMake Build options
# These options only affect the build if the built-type is Debug - otherwise they will be ignored
option(CP8_DEBUG_PRINT_BYTECODE "Determines whether the instructions are printed" OFF)
option(CP8_DEBUG_TRACE_EXECUTION "Determines whether the execution shall be traced" OFF)

 # Adding CheckIncludeFile CMake module
include(CheckIncludeFile) 

# Checking dependencies
include(./cmake/chip8_check_dependencies.cmake)
CHIP8_Check_Dependencies()

# Build SDL
add_subdirectory(external/SDL)

include(./cmake/chip8_determine_compiler.cmake)
CHIP8_Determine_Compiler()

include(./cmake/chip8_detect_platform.cmake)
CHIP8_Detect_Platform()

# Build CHIP-8 project
add_subdirectory(src)
