cmake_minimum_required(VERSION 3.16)
project(CHIP8 VERSION 0.1.0)

# TODO: link with sdl libary

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
elseif(CMAKE_C_COMPILER_ID STREQUAL "[Cc][Ll][Aa][Nn][Gg]")
    add_compile_definitions(COMPILER_GCC)
elseif(CMAKE_C_COMPILER_ID STREQUAL "[Mm][Ss][Vv][Cc]")
    add_compile_definitions(COMPILER_MSVC)
else()
    add_compile_definitions(COMPILER_UNKNOWN)    
endif()

# Build CHIP8 project
add_subdirectory(src)