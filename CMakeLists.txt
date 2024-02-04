cmake_minimum_required(VERSION 3.14)
project(tcci)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/b3a9ba2b8e975550799838332803d468797ae2e1.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB LIB_SOURCES "src/*.c")
list(FILTER LIB_SOURCES EXCLUDE REGEX "src/repl\\.c")
list(FILTER LIB_SOURCES EXCLUDE REGEX "src/debug\\.c")

add_library(common_lib ${LIB_SOURCES})

add_executable(repl src/repl.c)
target_link_libraries(repl common_lib)

add_executable(debug src/debug.c)
target_link_libraries(debug common_lib)

file(GLOB TEST_SOURCES "tests/*.cpp")

add_executable(
  test_module
  ${TEST_SOURCES}
)
target_link_libraries(
  test_module
  common_lib
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_module)