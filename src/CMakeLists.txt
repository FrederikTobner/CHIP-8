# configure cellox config to show version
configure_file(chip8_config.h.in chip8_config.h)

set(CHIP-8_SOURCE_FILES
"assembler.c"
"chip8.c"
"debug.c"
"main.c"    
)

set(CHIP-8_HEADER_FILES
"assembler.c"
"chip8.h"   
"common.h"
"debug.h"
"flags.h"
"assembler.h"
)

add_executable(${PROJECT_NAME} ${CHIP-8_SOURCE_FILES} ${CHIP-8_HEADER_FILES})

# TODO: link with sdl libary

# for including the chip8_config.h file
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR}/src)

if(NOT CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
    include(CPack)    
    # Sets properties for the package created using cpack
    set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
    set(CPACK_PACKAGE_VERSION "${CHIP-8_VERSION_MAJOR}.${CHIP-8_VERSION_MINOR}.${CHIP-8_VERSION_PATCH}")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} Emulator")
    set(CPACK_BUILD_SOURCE_DIRS "${PROJECT_BINARY_DIR}/src")
    set(CPACK_PACKAGE_VENDOR ${PROJECT_VENDOR})
    
    # Sets properties for NSIS (nullsoft scriptable install system - https://sourceforge.net/projects/nsis/)
    set(CPACK_NSIS_MODIFY_PATH "ON")
    set(CPACK_NSIS_URL_INFO_ABOUT ${PROJECT_PAGE})
    set(CPACK_NSIS_HELP_LINK ${PROJECT_HELP_PAGE}) 
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
endif()
