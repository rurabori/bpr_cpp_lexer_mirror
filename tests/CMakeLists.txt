cmake_minimum_required(VERSION 3.10)

add_executable(
    tests 
    main.cpp test_action.cpp test_rule.cpp test_utils.cpp test_filter.cpp
)

add_custom_command(
    TARGET tests
    POST_BUILD
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tests
)

target_include_directories(tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third-party/compile-time-regular-expressions/include
    ${CMAKE_SOURCE_DIR}/third-party/catch2
)

target_compile_options(tests PRIVATE -s -fconcepts -std=c++2a -Ofast -march=skylake)
