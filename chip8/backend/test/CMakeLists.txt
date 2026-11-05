set(BACKEND_TEST_PROJECT_NAME ${PROJECT_NAME}_Backend_Tests)

# include google test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.18.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Set all test files
file(GLOB_RECURSE TEST_SOURCES "*.cpp")

add_executable(${BACKEND_TEST_PROJECT_NAME} ${TEST_SOURCES})

# The games in the examples folder are assembled and played in the tests
target_compile_definitions(${BACKEND_TEST_PROJECT_NAME} PRIVATE CP8_EXAMPLES_DIR="${PROJECT_SOURCE_DIR}/examples")

# Link google test
target_link_libraries(${BACKEND_TEST_PROJECT_NAME} GTest::gtest_main ${PROJECT_NAME}_Backend)

target_include_directories(${BACKEND_TEST_PROJECT_NAME} PRIVATE "../src/")

include(GoogleTest)
gtest_discover_tests(${BACKEND_TEST_PROJECT_NAME})
