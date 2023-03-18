if(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
    set(BACKEND_SOURCE_FILES
    "virtual_machine.c"
    "debug.c"
    "display.c"
    "keyboard_state.c"
    )

    set(BACKEND_HEADER_FILES
    "virtual_machine.h"
    "debug.h"
    "display.h"
    "keyboard_state.h"
    )
else()
    set(BACKEND_SOURCE_FILES
    "virtual_machine.c"
    "display.c"
    "keyboard_state.c"
    )

    set(BACKEND_HEADER_FILES
    "virtual_machine.h"
    "display.h"
    "keyboard_state.h"
    )
endif()

add_library(${PROJECT_NAME}_Backend STATIC ${BACKEND_SOURCE_FILES} ${BACKEND_HEADER_FILES})

# Precompiles common.h to speed up compilation of the target
if(MSVC)
    # VisualStudio only accepts header files that also have a source file    
    target_precompile_headers(${PROJECT_NAME}_Backend PUBLIC backend_pre_compiled_header.h backend_pre_compiled_header.c) 
else()
    target_precompile_headers(${PROJECT_NAME}_Backend PUBLIC backend_pre_compiled_header.h)
endif()

target_link_libraries(${PROJECT_NAME}_Backend SDL2-static ${PROJECT_NAME}_Base ${PROJECT_NAME}_IO)