#include "antlr_test.h"
#include "ANTLRFileStream.h"

int main(int argc, char const *argv[])
{
    if (argc < 2) return 1;

    antlr4::ANTLRFileStream file{argv[1]};
    antlr_test lexer{&file};

    const auto& vocabulary = lexer.getVocabulary();
   
    while (auto token = lexer.nextToken()) {
        auto tok_name = vocabulary.getSymbolicName(token->getType());
        if (tok_name == "EOF")
            break;
        std::cout << tok_name  << ' ' << token->getText() << '\n';
    }


    return 0;
}

