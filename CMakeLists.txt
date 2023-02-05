cmake_minimum_required(VERSION 3.16)
include(CheckIncludeFile)

project(CHIP-8 VERSION 0.1.0 LANGUAGES C)

# C99 standard is required to build the emulator
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# CPack metadata
set(PROJECT_PAGE "https://github.com/FrederikTobner/CHIP-8")
set(PROJECT_HELP_PAGE "https://github.com/FrederikTobner/CHIP-8/wiki")
set(PROJECT_VENDOR "Frederik Tobner")

option(DEBUG_PRINT_BYTECODE "Determines whether the chunks are dissassembled and the bytecode is printed" ON)
option(DEBUG_TRACE_EXECUTION "Determines whether the execution shall be traced" OFF)

# Check dependecies under unix-like systems
if(UNIX)
    CHECK_INCLUDE_FILE("curses.h" CURSES_AVAILABLE)
    CHECK_INCLUDE_FILE("unistd.h" UNISTD_AVAILABLE)
    if(NOT ${CURSES_AVAILABLE})
        message(FATAL_ERROR "curses.h is required to build the emulator under unix-like systems. Please make sure it is available to the compiler and try again after that")
    endif() # curses.h not available
    if(NOT ${UNISTD_AVAILABLE})
        message(FATAL_ERROR "unistd.h is required to build the emulator under unix-like systems. Please make sure it is available to the compiler and try again after that")
    endif() # unistd.h not available
endif() # Unix system

# Check dependecies under windows
if(WIN32)
    CHECK_INCLUDE_FILE("conio.h" CONIO_AVAILABLE)
    CHECK_INCLUDE_FILE("windows.h" WINDOWS_AVAILABLE)
    if(NOT ${CONIO_AVAILABLE})
        message(FATAL_ERROR "conio.h is required to build the emulator under windows. Please make sure it is available to the compiler and try again after that")
    endif() # conio.h not available
    if(NOT ${WINDOWS_AVAILABLE})
        message(FATAL_ERROR "windows.h is required to build the emulator under windows. Please make sure it is available to the compiler and try again after that")
    endif() # windows.h not available
endif() # Windows system

# Build SDL
add_subdirectory(external/SDL)

# Adding os-specific compiler definitions
if(UNIX)
    add_compile_definitions(OS_UNIX_LIKE)    
    if(LINUX) 
        add_compile_definitions(OS_LINUX)
    elseif(APPLE) 
        add_compile_definitions(OS_MACOS)
    else()
        add_compile_definitions(OS_UNKNOWN_UNIX)
    endif()
elseif(WIN32)  
    add_compile_definitions(OS_WINDOWS)
else()
    add_compile_definitions(OS_UNKNOWN)
endif()

# We determine the compiler so we can do some optimization for a specific compiler
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_definitions(COMPILER_GCC)
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    add_compile_definitions(COMPILER_CLANG)
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    add_compile_definitions(COMPILER_MSVC)
else()
    add_compile_definitions(COMPILER_UNKNOWN)    
endif()

# Build CHIP-8 project
add_subdirectory(src)
