# configure cellox config to show version
configure_file(chip8_config.h.in chip8_config.h)

set(CHIP8_SOURCE_FILES
"main.c"    
)

add_executable(${PROJECT_NAME} ${CHIP8_SOURCE_FILES})

# for including the chip8_config.h file
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR}/src)