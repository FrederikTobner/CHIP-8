# configure cellox config to show version
configure_file(chip8_config.h.in chip8_config.h)

set(CHIP8_SOURCE_FILES
"chip8.c"
"debug.c"
"lexer.c"
"main.c"    
)

set(CHIP8_HEADER_FILES
"chip8.h"   
"common.h"
"debug.h"
"flags.h"
"lexer.h"
)

add_executable(${PROJECT_NAME} ${CHIP8_SOURCE_FILES})

# for including the chip8_config.h file
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR}/src)