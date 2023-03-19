set(FRONTEND_SOURCE_FILES
"address_hash_table.c"
"addresses_hash_table.c"
"assembler.c"
"dynamic_address_array.c"
"fnv1a.c"
)

set(FRONTEND_HEADER_FILES
"address_hash_table.h"
"addresses_hash_table.h"
"assembler.h"
"array_code_gen.h"
"dynamic_address_array.h"
"fnv1a.h"
"memory.h"
"table.h"
)

add_library(${PROJECT_NAME}_Frontend STATIC ${FRONTEND_SOURCE_FILES} ${FRONTEND_HEADER_FILES})

target_link_libraries(${PROJECT_NAME}_Frontend ${PROJECT_NAME}_Base)

# Precompiles common.h to speed up compilation of the target
if(MSVC)
    # VisualStudio only accepts header files that also have a source file    
    target_precompile_headers(${PROJECT_NAME}_Frontend PUBLIC frontend_pre_compiled_header.h frontend_pre_compiled_header.c) 
else()
    target_precompile_headers(${PROJECT_NAME}_Frontend PUBLIC frontend_pre_compiled_header.h)
endif()
