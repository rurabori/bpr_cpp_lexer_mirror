cmake_minimum_required(VERSION 3.10)
project(flex_cpp_lexer VERSION 0.0.1)

add_custom_command(
    OUTPUT flex_cpp_lexer.cpp
    COMMAND flex    -o${CMAKE_CURRENT_BINARY_DIR}/flex_cpp_lexer.cpp 
                    ${CMAKE_CURRENT_SOURCE_DIR}/flex_cpp_lexer.lex
    DEPENDS flex_cpp_lexer.lex
)

add_executable(flex_cpp_lexer ${CMAKE_CURRENT_BINARY_DIR}/flex_cpp_lexer.cpp)

target_include_directories(flex_cpp_lexer PRIVATE
    ../third-party/wise_enum
    ../common
)
#compiler setup
#set_property(TARGET ctle PROPERTY CXX_STANDARD 2a) my cmake doesn't support 20.
target_compile_options(flex_cpp_lexer PRIVATE -std=c++2a -O2)