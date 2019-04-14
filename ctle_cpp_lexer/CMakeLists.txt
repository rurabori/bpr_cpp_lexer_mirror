cmake_minimum_required(VERSION 3.10)
project(ctle_cpp_lexer VERSION 0.0.1)

add_executable(ctle_cpp_lexer main.cpp)

target_include_directories(ctle_cpp_lexer PRIVATE
    ../third-party/compile-time-regular-expressions/include
    ../third-party/wise_enum
    ../ctle/include
    ../common
)
#compiler setup
#set_property(TARGET ctle PROPERTY CXX_STANDARD 2a) my cmake doesn't support 20.
target_compile_options(ctle_cpp_lexer PRIVATE -std=c++2a -fconcepts -O2)
#ctre specific flag
target_compile_definitions(ctle_cpp_lexer PRIVATE EXPERIMENTAL_GCC_9)