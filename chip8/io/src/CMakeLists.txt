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

# Precompiles common.h to speed up compilation of the target
if(MSVC)
    # VisualStudio only accepts header files that also have a source file    
    target_precompile_headers(${PROJECT_NAME}_IO PUBLIC io_pre_compiled_header.h io_pre_compiled_header.c) 
else()
    target_precompile_headers(${PROJECT_NAME}_IO PUBLIC io_pre_compiled_header.h)
endif()