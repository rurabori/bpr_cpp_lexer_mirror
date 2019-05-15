#include "lexer.h"
#include "rule.h"
#include "file.h"
#include <cstdio>

enum class tokens
{
    INT = ctle::state_reserved,
    PRINT,
    no_match,
    eof
};

int main(int argc, char const* argv[]) {
    using rule_list
      = ctll::list<ctle::rule<"[[:digit:]]+", [](auto&&...) { return tokens::INT; }>,
                   ctle::rule<"[ \t\r\n]+">,
                   ctle::rule<"print:'(.*?)'",
                              [](auto& lexer, auto lexeme, auto to_print) -> std::optional<tokens> {
                                  lexer.set_state(ctle::state_initial);
                                  if (!to_print.length()) return std::nullopt;
                                  printf("%s", to_print.begin());
                                  return tokens::PRINT;
                              }>>;

    using lexer_t = ctle::lexer<tokens, rule_list>;

    lexer_t lexer;

    ctle::basic_file<char> input{argv[1]};
    lexer.set_input(input);

    while (true)
        if (auto [token, lexeme] = lexer.lex(); token == tokens::no_match || token == tokens::eof)
            break;

    return 0;
}
