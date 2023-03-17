
set(CHIP-8_BASE_HEADER_FILES
"alias.h"
"exit_code.h"
)

add_library(${PROJECT_NAME}_Base INTERFACE ${CHIP-8_BASE_HEADER_FILES})
set_target_properties(${PROJECT_NAME}_Base PROPERTIES LINKER_LANGUAGE C)