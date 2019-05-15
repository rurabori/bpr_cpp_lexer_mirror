cmake_minimum_required(VERSION 3.10)

add_custom_command(
    OUTPUT antlr_cpp_lexer.cpp antlr_cpp_lexer.h
    COMMAND java    -jar /usr/local/lib/antlr.jar 
                    -Dlanguage=Cpp 
                    -o ${CMAKE_CURRENT_BINARY_DIR} 
                    # java requires the absolute path as it is probably started elsewhere
                    ${CMAKE_CURRENT_SOURCE_DIR}/antlr_cpp_lexer.g4
    #antlr insists on using the string literal, gcc doesn't like.
    COMMAND sed -i -e "s/u8\"/\"/g" 
                ${CMAKE_CURRENT_BINARY_DIR}/antlr_cpp_lexer.cpp
    DEPENDS antlr_cpp_lexer.g4
)
add_executable(antlr_cpp_lexer ${CMAKE_CURRENT_BINARY_DIR}/antlr_cpp_lexer.cpp main.cpp)

target_include_directories(
    antlr_cpp_lexer PRIVATE
        /usr/local/include/antlr4-runtime 
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(antlr_cpp_lexer PRIVATE /usr/local/lib/libantlr4-runtime.a)
target_compile_options(antlr_cpp_lexer PRIVATE -std=c++2a -Ofast -march=skylake)
