cmake_minimum_required(VERSION 3.10)

add_executable(ctle_minimal main.cpp)

target_include_directories(ctle_minimal PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third-party/compile-time-regular-expressions/include
)

target_compile_options(ctle_minimal PRIVATE -s -fconcepts -std=c++2a -Ofast -march=skylake)
