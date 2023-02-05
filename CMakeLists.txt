# At least CMake >= Version 3.16 is required
cmake_minimum_required(VERSION 3.16)

# Defining project name, version and used languages
project(CHIP-8 VERSION 0.1.0 LANGUAGES C)

# C99 standard is required to build the emulator
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Setting project properties
set(PROJECT_PAGE "https://github.com/FrederikTobner/CHIP-8")
set(PROJECT_HELP_PAGE "https://github.com/FrederikTobner/CHIP-8/wiki")
set(PROJECT_VENDOR "Frederik Tobner")

# CMake Build options

# These options only affect the build if the built-type is Debug - otherwise they will be ignored
option(CP8_DEBUG_PRINT_BYTECODE "Determines whether the chunks are dissassembled and the bytecode is printed" OFF)
option(CP8_DEBUG_TRACE_EXECUTION "Determines whether the execution shall be traced" OFF)

# Adding CheckIncludeFile CMake module
include(CheckIncludeFile)


if(UNIX)
    # Adding os-specific compiler definitions
    add_compile_definitions(OS_UNIX_LIKE)    
    if(LINUX) 
        add_compile_definitions(OS_LINUX)
    elseif(APPLE) 
        add_compile_definitions(OS_MACOS)
    else()
        add_compile_definitions(OS_UNKNOWN_UNIX)
    endif() 
    # Check dependecies under unix-like systems   
    CHECK_INCLUDE_FILE("curses.h" CURSES_AVAILABLE)
    CHECK_INCLUDE_FILE("unistd.h" UNISTD_AVAILABLE)
    if(NOT ${CURSES_AVAILABLE})
        message(FATAL_ERROR "curses.h is required to build the emulator under unix-like systems. \
\   \   Please make sure it is available to the compiler and try again after that")
    endif() # curses.h
    if(NOT ${UNISTD_AVAILABLE})
        message(FATAL_ERROR "unistd.h is required to build the emulator under unix-like systems. \
\   \   Please make sure it is available to the compiler and try again after that")
    endif() # unistd.h
elseif(WIN32)
    # Adding os-specific compiler definitions
    add_compile_definitions(OS_WINDOWS)
    # Check dependecies under windows
    CHECK_INCLUDE_FILE("conio.h" CONIO_AVAILABLE)
    CHECK_INCLUDE_FILE("windows.h" WINDOWS_AVAILABLE)
    if(NOT ${CONIO_AVAILABLE})
        message(FATAL_ERROR "conio.h is required to build the emulator under windows. \
\   \   Please make sure it is available to the compiler and try again after that")
    endif() # conio.h
    if(NOT ${WINDOWS_AVAILABLE})
        message(FATAL_ERROR "windows.h is required to build the emulator under windows. \
\   \   Please make sure it is available to the compiler and try again after that"
        )
    endif() # windows.h
else()
    # Adding os-specific compiler definitions
    add_compile_definitions(OS_UNKNOWN)
endif()

# Build SDL
add_subdirectory(external/SDL)

# We determine the compiler so we can do some optimization for a specific compiler
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_definitions(COMPILER_GCC)
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    add_compile_definitions(COMPILER_CLANG)
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    add_compile_definitions(COMPILER_MSVC)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    add_compile_definitions(COMPILER_INTEL)
else()
    add_compile_definitions(COMPILER_UNKNOWN)    
endif()

# Build CHIP-8 project
add_subdirectory(src)
