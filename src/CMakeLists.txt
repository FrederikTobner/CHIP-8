# configure chip-8 config to show version
configure_file(chip8_config.h.in chip8_config.h)

# Adding debug compile definitions
if(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
    set(CHIP-8_SOURCE_FILES
    "frontend/address_hash_table.c"
    "frontend/assembler.c"
    "backend/chip8.c"
    "backend/debug.c"
    "backend/display.c"
    "utils/file_utils.c"
    "frontend/fnv1a.c"
    "main.c"    
    "utils/path_utils.c"
    )    
    set(CHIP-8_HEADER_FILES
    "frontend/address_hash_table.h"
    "frontend/assembler.h"
    "backend/chip8.h"
    "backend/debug.h"
    "backend/display.h"
    "utils/file_utils.h"
    "frontend/fnv1a.h"
    "utils/path_utils.h"
    )
    if(CP8_DEBUG_PRINT_BYTECODE)
        add_compile_definitions(PRINT_BYTE_CODE)
    endif()
    if(CP8_DEBUG_TRACE_EXECUTION)
        add_compile_definitions(TRACE_EXECUTION)
    endif()
    add_compile_definitions(BUILD_DEBUG)
else()
    set(CHIP-8_SOURCE_FILES
    "frontend/address_hash_table.c"
    "frontend/assembler.c"
    "backend/chip8.c"
    "backend/display.c"
    "utils/file_utils.c"
    "frontend/fnv1a.c"
    "main.c"    
    "utils/path_utils.c"
    )    
    set(CHIP-8_HEADER_FILES
    "frontend/address_hash_table.h"
    "frontend/assembler.h"
    "backend/chip8.h"
    "backend/display.h"
    "utils/file_utils.h"
    "frontend/fnv1a.h"
    "utils/path_utils.h"
    )

    include(../cmake/chip8_set_package_properties.cmake)
    CHIP8_Set_Package_Properties()

    # Includes the CPack module     
    include(CPack)
endif()

# Creating the final executable
add_executable(${PROJECT_NAME} ${CHIP-8_SOURCE_FILES} ${CHIP-8_HEADER_FILES})

# Precompiles common.h to speed up compilation of the target
if(MSVC)
    # VisualStudio only accepts header files that also have a source file    
    target_precompile_headers(${PROJECT_NAME} PUBLIC pre_compiled_header.h pre_compiled_header.c) 
else()
    target_precompile_headers(${PROJECT_NAME} PUBLIC pre_compiled_header.h)
endif()

# for including the chip8_config.h file
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR}/src)

# Linking with SDL libary
target_link_libraries(${PROJECT_NAME} SDL2main SDL2-static)

# Copies window icon into the build folder
FILE(COPY ${PROJECT_SOURCE_DIR}/icons/chip8_window_icon.bmp DESTINATION "${CMAKE_BINARY_DIR}/icons")

if(NOT CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
    # Install destinations
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
    # Adding window icon
    install(FILES ${PROJECT_SOURCE_DIR}/icons/chip8_window_icon.bmp DESTINATION icons)
endif()
