include(../cmake/chip8_check.cmake)

CHIP8_Check_Compiler()

CHIP8_Check_Platform()

if(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
    add_compile_definitions(BUILD_TYPE_DEBUG)
endif()

add_subdirectory(base)

add_subdirectory(io)

add_subdirectory(backend)

add_subdirectory(frontend)

add_subdirectory(main)
