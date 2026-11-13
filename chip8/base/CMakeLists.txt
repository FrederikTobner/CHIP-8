set(CHIP-8_BASE_HEADER_FILES
"${CMAKE_CURRENT_SOURCE_DIR}/include/base/chip8.h"
"${CMAKE_CURRENT_SOURCE_DIR}/include/base/dynamic_array_code_gen.h"
"${CMAKE_CURRENT_SOURCE_DIR}/include/base/exit_code.h"
"${CMAKE_CURRENT_SOURCE_DIR}/include/base/logger.h"
"${CMAKE_CURRENT_SOURCE_DIR}/include/base/memory.h"
"${CMAKE_CURRENT_SOURCE_DIR}/include/base/table_code_gen.h"
"${CMAKE_CURRENT_SOURCE_DIR}/include/base/table.h"
)

add_library(${PROJECT_NAME}_Base INTERFACE)
target_sources(${PROJECT_NAME}_Base INTERFACE ${CHIP-8_BASE_HEADER_FILES})
# The headers are included as "base/<header>.h"
target_include_directories(${PROJECT_NAME}_Base INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
