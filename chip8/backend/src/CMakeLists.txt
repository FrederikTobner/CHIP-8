if(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
    set(BACKEND_SOURCE_FILES
    "virtual_machine.c"
    "debug.c"
    "display.c"
    "keyboard_state.c"
    )
else()
    set(BACKEND_SOURCE_FILES
    "virtual_machine.c"
    "display.c"
    "keyboard_state.c"
    )
endif()

# The window icon is embedded in the library as an array of bytes, so it does not have to be found at runtime
set(WINDOW_ICON_FILE ${PROJECT_SOURCE_DIR}/assets/chip8_window_icon.bmp)
# Configures the project again when the icon changes
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${WINDOW_ICON_FILE})
file(READ ${WINDOW_ICON_FILE} WINDOW_ICON_HEX HEX)
# Every byte becomes a C literal (0x42, ...) and every row contains 16 of them (CMake regular expressions do not support
# repetition counts, so the pattern of a row is built first)
string(REPEAT "[0-9a-f][0-9a-f]" 16 WINDOW_ICON_ROW)
string(REGEX REPLACE "(${WINDOW_ICON_ROW})" "\\1\n" WINDOW_ICON_ROWS "${WINDOW_ICON_HEX}")
string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " WINDOW_ICON_BYTES "${WINDOW_ICON_ROWS}")
string(REPLACE "\n" "\n    " WINDOW_ICON_BYTES "${WINDOW_ICON_BYTES}")
configure_file(window_icon.h.in window_icon.h @ONLY)

add_library(${PROJECT_NAME}_Backend STATIC ${BACKEND_SOURCE_FILES})

# The public headers are included as "backend/<header>.h"
target_include_directories(${PROJECT_NAME}_Backend PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
# The generated window_icon.h is only used inside of the library
target_include_directories(${PROJECT_NAME}_Backend PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if(MSVC)
    # VisualStudio only accepts header files that also have a source file    
    target_precompile_headers(${PROJECT_NAME}_Backend PRIVATE backend_pre_compiled_header.h backend_pre_compiled_header.c) 
else()
    target_precompile_headers(${PROJECT_NAME}_Backend PRIVATE backend_pre_compiled_header.h)
endif()

target_link_libraries(${PROJECT_NAME}_Backend SDL2::SDL2 ${PROJECT_NAME}_Base ${PROJECT_NAME}_IO)