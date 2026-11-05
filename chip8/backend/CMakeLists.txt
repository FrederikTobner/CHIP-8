set(SOURCE_PATH_BACKEND ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(test)
endif()