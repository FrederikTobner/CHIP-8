if(CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]")
    set(BACKEND_SOURCE_FILES
    "chip8.c"
    "debug.c"
    "display.c"
    "path_utils.c"
    )

    set(BACKEND_HEADER_FILES
    "chip8.h"
    "debug.h"
    "display.h"
    "path_utils.h"
    )
else()
    set(BACKEND_SOURCE_FILES
    "chip8.c"
    "display.c"
    "path_utils.c"
    )

    set(BACKEND_HEADER_FILES
    "chip8.h"
    "display.h"
    "path_utils.h"
    )
endif()

add_library(${PROJECT_NAME}Backend STATIC ${BACKEND_SOURCE_FILES} ${BACKEND_HEADER_FILES})

target_link_libraries(${PROJECT_NAME}Backend SDL2-static)