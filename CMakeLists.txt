cmake_minimum_required(VERSION 3.0.0)
project(compile_time_lexer VERSION 0.1.0)

set (CMAKE_CXX_FLAGS "-std=c++2a -fconcepts -DEXPERIMENTAL_GCC_9")

add_executable(compile_time_lexer src/main.cpp)

target_include_directories(compile_time_lexer PUBLIC src/compile-time-regular-expressions/include)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
