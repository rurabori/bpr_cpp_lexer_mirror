cmake_minimum_required(VERSION 3.10)

add_executable(ctle_cpp_lexer_full main.cpp)

add_custom_command(
    TARGET ctle_cpp_lexer_full
    POST_BUILD
    COMMAND strip --strip-unneeded ctle_cpp_lexer_full -o ctle_cpp_lexer
)

target_include_directories(ctle_cpp_lexer_full PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ../common
    ${CMAKE_SOURCE_DIR}/third-party/wise_enum
    ${CMAKE_SOURCE_DIR}/third-party/compile-time-regular-expressions/include
)

#compiler setup
#set_property(TARGET ctle PROPERTY CXX_STANDARD 2a) my cmake doesn't support 20.
target_compile_options(ctle_cpp_lexer_full PRIVATE -s -fconcepts -std=c++2a -Ofast -march=skylake)