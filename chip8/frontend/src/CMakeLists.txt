set(GLOBAL PROPERTY SOURCE_PATH_FRONTEND ${CMAKE_CURRENT_SOURCE_DIR})

set(FRONTEND_SOURCE_FILES
"address_hash_table.c"
"addresses_hash_table.c"
"assembler.c"
"dynamic_address_array.c"
"fnv1a.c"
)

add_library(${PROJECT_NAME}_Frontend STATIC ${FRONTEND_SOURCE_FILES}) 

target_link_libraries(${PROJECT_NAME}_Frontend ${PROJECT_NAME}_Base)

if(MSVC)
    # VisualStudio only accepts header files that also have a source file    
    target_precompile_headers(${PROJECT_NAME}_Frontend PUBLIC frontend_pre_compiled_header.h frontend_pre_compiled_header.c) 
else()
    target_precompile_headers(${PROJECT_NAME}_Frontend PUBLIC frontend_pre_compiled_header.h)
endif()
