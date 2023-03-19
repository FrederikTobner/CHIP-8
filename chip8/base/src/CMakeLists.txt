
set(CHIP-8_BASE_HEADER_FILES
"chip8.h"
"dynamic_array_code_gen.h"
"exit_code.h"
"logger.h"
"memory.h"
"table_code_gen.h"
"table.h"
)

add_library(${PROJECT_NAME}_Base INTERFACE ${CHIP-8_BASE_HEADER_FILES})
set_target_properties(${PROJECT_NAME}_Base PROPERTIES LINKER_LANGUAGE C)