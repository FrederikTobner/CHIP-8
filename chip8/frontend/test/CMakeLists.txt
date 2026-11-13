set(FRONTEND_TEST_PROJECT_NAME ${PROJECT_NAME}_Frontend_Tests)

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

file (GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "*.cpp")

add_executable(${FRONTEND_TEST_PROJECT_NAME} ${TEST_SOURCES})

# The test of the hash function also needs the header that is not part of the public interface of the library
target_include_directories(${FRONTEND_TEST_PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/chip8/frontend/src)

# Link google test
target_link_libraries(${FRONTEND_TEST_PROJECT_NAME} GTest::gtest_main ${PROJECT_NAME}_Frontend)

include(GoogleTest)
gtest_discover_tests(${FRONTEND_TEST_PROJECT_NAME})
