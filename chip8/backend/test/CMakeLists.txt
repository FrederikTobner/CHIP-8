set(BACKEND_TEST_PROJECT_NAME ${PROJECT_NAME}_Backend_Tests)

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
set(TEST_SOURCES main.cpp breakout.cpp display.cpp keyboard_state.cpp opcodes.cpp virtual_machine.cpp)

add_executable(${BACKEND_TEST_PROJECT_NAME} ${TEST_SOURCES})

# The games in the examples folder are assembled and played in the tests
target_compile_definitions(${BACKEND_TEST_PROJECT_NAME} PRIVATE CP8_EXAMPLES_DIR="${PROJECT_SOURCE_DIR}/examples")

# Link google test
target_link_libraries(${BACKEND_TEST_PROJECT_NAME} GTest::gtest_main ${PROJECT_NAME}_Backend ${PROJECT_NAME}_Frontend
                      ${PROJECT_NAME}_IO)

include(GoogleTest)
gtest_discover_tests(${BACKEND_TEST_PROJECT_NAME})
