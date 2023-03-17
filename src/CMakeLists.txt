include(../cmake/chip8_check.cmake)

# Determine the compiler that is used
CHIP8_Check_Compiler()

# Detect the platform that we build for
CHIP8_Check_Platform()

add_subdirectory(base)

add_subdirectory(io)

add_subdirectory(backend)

add_subdirectory(frontend)

add_subdirectory(main)