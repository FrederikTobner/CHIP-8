set(CHIP-8_IO_SOURCE_FILES
"file_utils.c"
"path_utils.c"   
)    
set(CHIP-8_IO_HEADER_FILES
"file_utils.h"
"path_utils.h"
)

add_library(${PROJECT_NAME}_IO STATIC ${CHIP-8_IO_SOURCE_FILES} ${CHIP-8_IO_HEADER_FILES})

target_link_libraries(${PROJECT_NAME}_IO ${PROJECT_NAME}_Base)