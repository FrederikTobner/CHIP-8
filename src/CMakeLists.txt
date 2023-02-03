# configure cellox config to show version
configure_file(chip8_config.h.in chip8_config.h)

set(CHIP8_SOURCE_FILES
"chip8.c"
"common.h"
"lexer.c"
"main.c"    
)

set(CHIP8_HEADER_FILES
"lexer.h"
"chip8.h"   
)

if(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
    if(DEBUG_PRINT_BYTECODE)
        add_compile_definitions(DEBUG_PRINT_CODE)
    endif()
    if(DEBUG_TRACE_EXECUTION)
        add_compile_definitions(DEBUG_TRACE_EXECUTION)
    endif()
    add_compile_definitions(BUILD_DEBUG)
endif()

add_executable(${PROJECT_NAME} ${CHIP8_SOURCE_FILES})

# for including the chip8_config.h file
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR}/src)