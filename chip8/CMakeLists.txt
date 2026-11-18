include(../cmake/chip8_check.cmake)

CHIP8_Check_Compiler()

CHIP8_Check_Platform()

if(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
    add_compile_definitions(BUILD_TYPE_DEBUG)
    # The debug options have to be added here, before the libraries are added - add_compile_definitions only affects the
    # targets of the current directory and the directories that are added afterwards
    if(CP8_DEBUG_PRINT_BYTECODE)
        add_compile_definitions(PRINT_BYTE_CODE)
    endif()
    if(CP8_DEBUG_TRACE_EXECUTION)
        add_compile_definitions(TRACE_EXECUTION)
    endif()
endif()

add_subdirectory(base)

add_subdirectory(io)

add_subdirectory(backend)

add_subdirectory(frontend)

add_subdirectory(main)
