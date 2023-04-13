set(FRONTEND_TEST_PROJECT_NAME ${PROJECT_NAME}_Frontend_Tests)

# include google test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Set all test files
set(TEST_SOURCES fnv1a.cpp main.cpp)

add_executable(${FRONTEND_TEST_PROJECT_NAME} ${TEST_SOURCES})

# Includes source path of the math library
target_include_directories(${FRONTEND_TEST_PROJECT_NAME} PUBLIC ${SOURCE_PATH_MATH})

# Link google test
target_link_libraries(${FRONTEND_TEST_PROJECT_NAME} GTest::gtest_main ${PROJECT_NAME}_Frontend)

include(GoogleTest)
gtest_discover_tests(${FRONTEND_TEST_PROJECT_NAME})
