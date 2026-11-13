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

add_library(${PROJECT_NAME}_Backend STATIC ${BACKEND_SOURCE_FILES})

# The public headers are included as "backend/<header>.h"
target_include_directories(${PROJECT_NAME}_Backend PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

if(MSVC)
    # VisualStudio only accepts header files that also have a source file    
    target_precompile_headers(${PROJECT_NAME}_Backend PRIVATE backend_pre_compiled_header.h backend_pre_compiled_header.c) 
else()
    target_precompile_headers(${PROJECT_NAME}_Backend PRIVATE backend_pre_compiled_header.h)
endif()

target_link_libraries(${PROJECT_NAME}_Backend SDL2::SDL2 ${PROJECT_NAME}_Base ${PROJECT_NAME}_IO)