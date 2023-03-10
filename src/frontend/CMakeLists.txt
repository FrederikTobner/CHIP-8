set(FRONTEND_SOURCE_FILES
"address_hash_table.c"
"assembler.c"
"fnv1a.c"
)

set(FRONTEND_HEADER_FILES
"address_hash_table.h"
"assembler.h"
"fnv1a.h"
)

add_library(${PROJECT_NAME}Frontend STATIC ${FRONTEND_SOURCE_FILES} ${FRONTEND_HEADER_FILES})

# Precompiles common.h to speed up compilation of the target
if(MSVC)
    # VisualStudio only accepts header files that also have a source file    
    target_precompile_headers(${PROJECT_NAME}Frontend PUBLIC pre_compiled_header.h pre_compiled_header.c) 
else()
    target_precompile_headers(${PROJECT_NAME}Frontend PUBLIC pre_compiled_header.h)
endif()
