cmake_minimum_required(VERSION 3.14)
project(tcci C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

enable_testing()

function(enable_strict_warnings target_name)
  if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
  elseif(MSVC)
    target_compile_options(${target_name} PRIVATE /W4)
  endif()
endfunction()

add_library(common_lib
  src/lexer.c
  src/parser.c
  src/interpreter.c
)
target_include_directories(common_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
enable_strict_warnings(common_lib)

add_executable(repl src/repl.c)
target_link_libraries(repl PRIVATE common_lib)
enable_strict_warnings(repl)

add_executable(debug src/debug.c)
target_link_libraries(debug PRIVATE common_lib)
enable_strict_warnings(debug)

set(TEST_LIBS common_lib)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  list(APPEND TEST_LIBS m)
endif()

add_executable(test_lexer tests/test_lexer.c)
target_link_libraries(test_lexer PRIVATE ${TEST_LIBS})
enable_strict_warnings(test_lexer)
add_test(NAME test_lexer COMMAND test_lexer)

add_executable(test_parser tests/test_parser.c)
target_link_libraries(test_parser PRIVATE ${TEST_LIBS})
enable_strict_warnings(test_parser)
add_test(NAME test_parser COMMAND test_parser)

add_executable(test_interpreter tests/test_interpreter.c)
target_link_libraries(test_interpreter PRIVATE ${TEST_LIBS})
enable_strict_warnings(test_interpreter)
add_test(NAME test_interpreter COMMAND test_interpreter)

add_executable(test_integration tests/test_integration.c)
target_link_libraries(test_integration PRIVATE ${TEST_LIBS})
enable_strict_warnings(test_integration)
add_test(NAME test_integration COMMAND test_integration)
