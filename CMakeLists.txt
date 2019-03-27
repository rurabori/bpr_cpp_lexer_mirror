cmake_minimum_required(VERSION 3.0.0)
project(compile_time_lexer VERSION 0.1.0)

set (CMAKE_CXX_FLAGS "-std=c++2a -fconcepts -DEXPERIMENTAL_GCC_9 -O2")

include_directories(    third-party/compile-time-regular-expressions/include
                        third-party/wise_enum
                        src/include
                        common
                    )
link_libraries(stdc++fs)

add_executable(compile_time_lexer src/main.cpp)

add_executable(elsa src/c_elsa_lexer.cpp)

add_executable(tests src/tests/test_filter.cpp)


add_custom_command(
    OUTPUT flex_test.cpp
    COMMAND flex -o${CMAKE_CURRENT_BINARY_DIR}/flex_test.cpp flex_test.lex
    DEPENDS ${CMAKE_SOURCE_DIR}/flex/flex_test.lex
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/flex
)
add_executable(flex_test ${CMAKE_CURRENT_BINARY_DIR}/flex_test.cpp)

add_custom_command(
    OUTPUT antlr_test.cpp antlr_test.h
    COMMAND java -jar /usr/local/lib/antlr.jar -Dlanguage=Cpp -o ${CMAKE_CURRENT_BINARY_DIR} antlr_test.g4
    #antlr insists on using the string literal, gcc doesn't like.
    COMMAND sed -i -e "s/u8\"/\"/g" ${CMAKE_CURRENT_BINARY_DIR}/antlr_test.cpp
    DEPENDS ${CMAKE_SOURCE_DIR}/antlr/antlr_test.g4
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/antlr
)
add_executable(antlr_test ${CMAKE_CURRENT_BINARY_DIR}/antlr_test.cpp antlr/antlr_main.cpp)
target_include_directories(antlr_test PRIVATE /usr/local/include/antlr4-runtime ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(antlr_test PRIVATE /usr/local/lib/libantlr4-runtime.a)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
