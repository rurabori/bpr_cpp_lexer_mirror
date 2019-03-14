cmake_minimum_required(VERSION 3.0.0)
project(compile_time_lexer VERSION 0.1.0)

set (CMAKE_CXX_FLAGS "-std=c++2a -fconcepts -DEXPERIMENTAL_GCC_9 -O2")

add_executable(compile_time_lexer src/main.cpp)
target_link_libraries(compile_time_lexer stdc++fs)
target_include_directories(compile_time_lexer PUBLIC src/compile-time-regular-expressions/include)
target_include_directories(compile_time_lexer PUBLIC src/wise_enum)
target_include_directories(compile_time_lexer PUBLIC src/include)

add_executable(elsa src/c_elsa_lexer.cpp)
target_link_libraries(elsa stdc++fs)
target_include_directories(elsa PUBLIC src/compile-time-regular-expressions/include)
target_include_directories(elsa PUBLIC src/wise_enum)
target_include_directories(elsa PUBLIC src/include)


add_executable(tests src/tests/test_filter.cpp)
target_link_libraries(tests stdc++fs)
target_include_directories(tests PUBLIC src)
target_include_directories(tests PUBLIC src/compile-time-regular-expressions/include)
target_include_directories(tests PUBLIC src/wise_enum)
target_include_directories(tests PUBLIC src/include)




set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
