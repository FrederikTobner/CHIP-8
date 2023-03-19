# configure chip-8 config to show version
configure_file(chip8_config.h.in chip8_config.h)

set(CHIP-8_SOURCE_FILES
"main.c"    
)

# Adding debug compile definitions
if(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")

    if(CP8_DEBUG_PRINT_BYTECODE)
        add_compile_definitions(PRINT_BYTE_CODE)
    endif()
    if(CP8_DEBUG_TRACE_EXECUTION)
        add_compile_definitions(TRACE_EXECUTION)
    endif()
    add_compile_definitions(BUILD_DEBUG)
else()
    include(../../../cmake/chip8_set_package_properties.cmake)
    CHIP8_Set_Package_Properties()
    # Includes the CPack module     
    include(CPack)
endif()

# Creating the final executable
add_executable(${PROJECT_NAME} ${CHIP-8_SOURCE_FILES})

# for including the chip8_config.h file
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR}/src)

# Linking with SDL libary and the front and backend libary of the emulators
target_link_libraries(${PROJECT_NAME} SDL2main ${PROJECT_NAME}_Backend ${PROJECT_NAME}_Frontend ${PROJECT_NAME}_Base ${PROJECT_NAME}_IO)

# Copies window icon into the build folder
FILE(COPY ${PROJECT_SOURCE_DIR}/assets/chip8_window_icon.bmp DESTINATION "${CMAKE_BINARY_DIR}/chip8/main/assets")

if(NOT CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
    # Install destinations
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
    # Adding window icon
    install(FILES ${PROJECT_SOURCE_DIR}/assets/chip8_window_icon.bmp DESTINATION icons)
endif()
