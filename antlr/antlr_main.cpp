#include "antlr_test.h"
#include "ANTLRFileStream.h"

int main(int argc, char const *argv[])
{
    if (argc < 2) return 1;

    antlr4::ANTLRFileStream file{argv[1]};
    antlr_test lexer{&file};

    const auto& vocabulary = lexer.getVocabulary();
   
    while (auto token = lexer.nextToken()) {
        if (lexer.hitEOF)
            break;
        std::cout << vocabulary.getSymbolicName(token->getType()) << '\n';
    }


    return 0;
}

