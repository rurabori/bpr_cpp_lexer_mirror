cmake_minimum_required(VERSION 3.10)

add_custom_command(
    OUTPUT flex_cpp_lexer.cpp
    COMMAND flex    -o${CMAKE_CURRENT_BINARY_DIR}/flex_cpp_lexer.cpp 
                    ${CMAKE_CURRENT_SOURCE_DIR}/flex_cpp_lexer.lex
    DEPENDS flex_cpp_lexer.lex
)

add_executable(flex_cpp_lexer ${CMAKE_CURRENT_BINARY_DIR}/flex_cpp_lexer.cpp)

target_include_directories(flex_cpp_lexer PRIVATE
    ../common
    ${CMAKE_SOURCE_DIR}/third-party/wise_enum
)
#compiler setup
#set_property(TARGET ctle PROPERTY CXX_STANDARD 2a) my cmake doesn't support 20.
target_compile_options(flex_cpp_lexer PRIVATE -march=skylake -std=c++2a -Ofast)