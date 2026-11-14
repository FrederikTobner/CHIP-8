# configure chip-8 config to show version
configure_file(chip8_config.h.in chip8_config.h)

set(CHIP-8_SOURCE_FILES
"main.c"    
)
# Packaging is only set up for builds that are not debug builds
if(NOT CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
    include(chip8_set_package_properties)
    CHIP8_Set_Package_Properties()
    # Includes the CPack module     
    include(CPack)
endif()

# Creating the final executable
add_executable(${PROJECT_NAME} ${CHIP-8_SOURCE_FILES})

# For including the generated chip8_config.h file (configure_file writes it to the binary directory of this folder,
# independent of the name or the location of the build directory)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# Linking with SDL libary and the front and backend libary of the emulators
target_link_libraries(${PROJECT_NAME} SDL2::SDL2main ${PROJECT_NAME}_Backend ${PROJECT_NAME}_Frontend ${PROJECT_NAME}_Base ${PROJECT_NAME}_IO)

if(NOT CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
    # Install destinations
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
endif()
