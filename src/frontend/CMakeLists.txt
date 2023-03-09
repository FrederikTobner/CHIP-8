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