#include "lexer.h"
#include "tokens.h"
#include <fstream>

namespace definition {
using namespace ctle;
using namespace ctle::default_actions;

class logger
{
public:
    WISE_ENUM_CLASS_MEMBER(levels, info, warning, error);
    void log(levels level, std::string_view message) {
        std::cout << "[" << wise_enum::to_string(level) << "] : " << message << '\n';
    }
};

class terminator
{
public:
    void terminate(const std::string& message = "") const { throw std::runtime_error(message); }
};

constexpr ctll::basic_fixed_string NL = "\n";
constexpr ctll::basic_fixed_string NOTNL = "[^\r\n]";
constexpr ctll::basic_fixed_string ANY = concat<"(?:", NOTNL, "|", NL, ")">();
constexpr ctll::basic_fixed_string BACKSL = "\\\\"; // escape the first with the second.
constexpr ctll::basic_fixed_string BOL = "^";
constexpr ctll::basic_fixed_string EOL = "\n";
constexpr ctll::basic_fixed_string LETTER = "[A-Za-z_$]";
constexpr ctll::basic_fixed_string ALNUM = "[A-Za-z_$0-9]";
constexpr ctll::basic_fixed_string DIGIT = "[0-9]";
constexpr ctll::basic_fixed_string HEXDIGIT = "[0-9A-Fa-f]";
constexpr ctll::basic_fixed_string DIGITS = concat<"(?:", DIGIT, "+)">();
constexpr ctll::basic_fixed_string HEXDIGITS = concat<"(?:", HEXDIGIT, "+)">();
constexpr ctll::basic_fixed_string SIGN = concat<"(?:\\+|-)">();
constexpr ctll::basic_fixed_string ELL_SUFFIX = "(?:[lL][lL]?)";
constexpr ctll::basic_fixed_string INT_SUFFIX
  = concat<"(?:[uU]", ELL_SUFFIX, "?|", ELL_SUFFIX, "[uU]?)">();
constexpr ctll::basic_fixed_string FLOAT_SUFFIX = "[flFL]";
constexpr ctll::basic_fixed_string STRCHAR = concat<"[^\"\n", BACKSL, "]">();
constexpr ctll::basic_fixed_string ESCAPE = concat<"(?:", BACKSL, ANY, ")">();
constexpr ctll::basic_fixed_string QUOTE = "[\"]";
constexpr ctll::basic_fixed_string CCCHAR = concat<"[^\'\n", BACKSL, "]">();
constexpr ctll::basic_fixed_string TICK = "[\']";
constexpr ctll::basic_fixed_string SPTAB = "[ \t]";
constexpr ctll::basic_fixed_string PPCHAR = concat<"(?:[^", BACKSL, "\n]|", BACKSL, NOTNL, ")">();

constexpr bool allow_newline_string_lits = true;

enum states
{
    BUGGY_STRING_LIT = state_reserved
};

constexpr auto error_float_literal = [](LexerInterface& lexer, auto content) {
    lexer.log(logger::levels::error, "floating literal with no digits after the 'e'");
    return TOK_FLOAT_LITERAL;
};

constexpr auto error_string_literal = [](LexerInterface& lexer, auto... content) {
    lexer.log(logger::levels::error,
              "at EOF, unterminated string literal; support for newlines in string "
              "literals is presently turned on, maybe the missing quote should have "
              "been much earlier in the file?");
    lexer.terminate();
};

using state_definitions = ctll::list<state<BUGGY_STRING_LIT, true, error_string_literal> >;

using rules = ctll::list<
  lexer_rule<"asm", simple_return(TOK_ASM)>, lexer_rule<"auto", simple_return(TOK_AUTO)>,
  lexer_rule<"break", simple_return(TOK_BREAK)>, lexer_rule<"bool", simple_return(TOK_BOOL)>,
  lexer_rule<"case", simple_return(TOK_CASE)>, lexer_rule<"catch", simple_return(TOK_CATCH)>,
  lexer_rule<"cdecl", simple_return(TOK_CDECL)>, lexer_rule<"char", simple_return(TOK_CHAR)>,
  lexer_rule<"class", simple_return(TOK_CLASS)>, lexer_rule<"const", simple_return(TOK_CONST)>,
  lexer_rule<"const_cast", simple_return(TOK_CONST_CAST)>,
  lexer_rule<"continue", simple_return(TOK_CONTINUE)>,
  lexer_rule<"default", simple_return(TOK_DEFAULT)>,
  lexer_rule<"delete", simple_return(TOK_DELETE)>, lexer_rule<"do", simple_return(TOK_DO)>,
  lexer_rule<"double", simple_return(TOK_DOUBLE)>,
  lexer_rule<"dynamic_cast", simple_return(TOK_DYNAMIC_CAST)>,
  lexer_rule<"else", simple_return(TOK_ELSE)>, lexer_rule<"enum", simple_return(TOK_ENUM)>,
  lexer_rule<"explicit", simple_return(TOK_EXPLICIT)>,
  lexer_rule<"export", simple_return(TOK_EXPORT)>, lexer_rule<"extern", simple_return(TOK_EXTERN)>,
  lexer_rule<"false", simple_return(TOK_FALSE)>, lexer_rule<"float", simple_return(TOK_FLOAT)>,
  lexer_rule<"for", simple_return(TOK_FOR)>, lexer_rule<"friend", simple_return(TOK_FRIEND)>,
  lexer_rule<"goto", simple_return(TOK_GOTO)>, lexer_rule<"if", simple_return(TOK_IF)>,
  lexer_rule<"inline", simple_return(TOK_INLINE)>, lexer_rule<"int", simple_return(TOK_INT)>,
  lexer_rule<"long", simple_return(TOK_LONG)>, lexer_rule<"mutable", simple_return(TOK_MUTABLE)>,
  lexer_rule<"namespace", simple_return(TOK_NAMESPACE)>, lexer_rule<"new", simple_return(TOK_NEW)>,
  lexer_rule<"operator", simple_return(TOK_OPERATOR)>,
  lexer_rule<"pascal", simple_return(TOK_PASCAL)>,
  lexer_rule<"private", simple_return(TOK_PRIVATE)>,
  lexer_rule<"protected", simple_return(TOK_PROTECTED)>,
  lexer_rule<"public", simple_return(TOK_PUBLIC)>,
  lexer_rule<"register", simple_return(TOK_REGISTER)>,
  lexer_rule<"reinterpret_cast", simple_return(TOK_REINTERPRET_CAST)>,
  lexer_rule<"return", simple_return(TOK_RETURN)>, lexer_rule<"short", simple_return(TOK_SHORT)>,
  lexer_rule<"signed", simple_return(TOK_SIGNED)>, lexer_rule<"sizeof", simple_return(TOK_SIZEOF)>,
  lexer_rule<"static", simple_return(TOK_STATIC)>,
  lexer_rule<"static_cast", simple_return(TOK_STATIC_CAST)>,
  lexer_rule<"struct", simple_return(TOK_STRUCT)>, lexer_rule<"switch", simple_return(TOK_SWITCH)>,
  lexer_rule<"template", simple_return(TOK_TEMPLATE)>, lexer_rule<"this", simple_return(TOK_THIS)>,
  lexer_rule<"throw", simple_return(TOK_THROW)>, lexer_rule<"true", simple_return(TOK_TRUE)>,
  lexer_rule<"try", simple_return(TOK_TRY)>, lexer_rule<"typedef", simple_return(TOK_TYPEDEF)>,
  lexer_rule<"typeid", simple_return(TOK_TYPEID)>,
  lexer_rule<"typename", simple_return(TOK_TYPENAME)>,
  lexer_rule<"union", simple_return(TOK_UNION)>,
  lexer_rule<"unsigned", simple_return(TOK_UNSIGNED)>,
  lexer_rule<"using", simple_return(TOK_USING)>, lexer_rule<"virtual", simple_return(TOK_VIRTUAL)>,
  lexer_rule<"void", simple_return(TOK_VOID)>, lexer_rule<"volatile", simple_return(TOK_VOLATILE)>,
  lexer_rule<"wchar_t", simple_return(TOK_WCHAR_T)>, lexer_rule<"while", simple_return(TOK_WHILE)>,
  lexer_rule<"\\(", simple_return(TOK_LPAREN)>, lexer_rule<"\\)", simple_return(TOK_RPAREN)>,
  lexer_rule<"\\[", simple_return(TOK_LBRACKET)>, lexer_rule<"\\]", simple_return(TOK_RBRACKET)>,
  lexer_rule<"->", simple_return(TOK_ARROW)>, lexer_rule<"::", simple_return(TOK_COLONCOLON)>,
  lexer_rule<"\\.", simple_return(TOK_DOT)>, lexer_rule<"!", simple_return(TOK_BANG)>,
  lexer_rule<"~", simple_return(TOK_TILDE)>, lexer_rule<"\\+", simple_return(TOK_PLUS)>,
  lexer_rule<"-", simple_return(TOK_MINUS)>, lexer_rule<"\\+\\+", simple_return(TOK_PLUSPLUS)>,
  lexer_rule<"--", simple_return(TOK_MINUSMINUS)>, lexer_rule<"&", simple_return(TOK_AND)>,
  lexer_rule<"\\*", simple_return(TOK_STAR)>, lexer_rule<"\\.\\*", simple_return(TOK_DOTSTAR)>,
  lexer_rule<"->\\*", simple_return(TOK_ARROWSTAR)>, lexer_rule<"/", simple_return(TOK_SLASH)>,
  lexer_rule<"%", simple_return(TOK_PERCENT)>, lexer_rule<"<<", simple_return(TOK_LEFTSHIFT)>,
  lexer_rule<">>", simple_return(TOK_RIGHTSHIFT)>, lexer_rule<"<", simple_return(TOK_LESSTHAN)>,
  lexer_rule<"<=", simple_return(TOK_LESSEQ)>, lexer_rule<">", simple_return(TOK_GREATERTHAN)>,
  lexer_rule<">=", simple_return(TOK_GREATEREQ)>, lexer_rule<"==", simple_return(TOK_EQUALEQUAL)>,
  lexer_rule<"!=", simple_return(TOK_NOTEQUAL)>, lexer_rule<"\\^", simple_return(TOK_XOR)>,
  lexer_rule<"\\|", simple_return(TOK_OR)>, lexer_rule<"&&", simple_return(TOK_ANDAND)>,
  lexer_rule<"\\|\\|", simple_return(TOK_OROR)>, lexer_rule<"\\?", simple_return(TOK_QUESTION)>,
  lexer_rule<":", simple_return(TOK_COLON)>, lexer_rule<"=", simple_return(TOK_EQUAL)>,
  lexer_rule<"\\*=", simple_return(TOK_STAREQUAL)>, lexer_rule<"/=", simple_return(TOK_SLASHEQUAL)>,
  lexer_rule<"%=", simple_return(TOK_PERCENTEQUAL)>,
  lexer_rule<"\\+=", simple_return(TOK_PLUSEQUAL)>, lexer_rule<"-=", simple_return(TOK_MINUSEQUAL)>,
  lexer_rule<"&=", simple_return(TOK_ANDEQUAL)>, lexer_rule<"\\^=", simple_return(TOK_XOREQUAL)>,
  lexer_rule<"\\|=", simple_return(TOK_OREQUAL)>,
  lexer_rule<"<<=", simple_return(TOK_LEFTSHIFTEQUAL)>,
  lexer_rule<">>=", simple_return(TOK_RIGHTSHIFTEQUAL)>, lexer_rule<",", simple_return(TOK_COMMA)>,
  lexer_rule<"\\.\\.\\.", simple_return(TOK_ELLIPSIS)>,
  lexer_rule<";", simple_return(TOK_SEMICOLON)>, lexer_rule<"\\{", simple_return(TOK_LBRACE)>,
  lexer_rule<"\\}", simple_return(TOK_RBRACE)>, lexer_rule<"<%", simple_return(TOK_LBRACE)>,
  lexer_rule<"%>", simple_return(TOK_RBRACE)>, lexer_rule<"<:", simple_return(TOK_LBRACKET)>,
  lexer_rule<":>", simple_return(TOK_RBRACKET)>, lexer_rule<"and", simple_return(TOK_ANDAND)>,
  lexer_rule<"bitor", simple_return(TOK_OR)>, lexer_rule<"or", simple_return(TOK_OROR)>,
  lexer_rule<"xor", simple_return(TOK_XOR)>, lexer_rule<"compl", simple_return(TOK_TILDE)>,
  lexer_rule<"bitand", simple_return(TOK_AND)>, lexer_rule<"and_eq", simple_return(TOK_ANDEQUAL)>,
  lexer_rule<"or_eq", simple_return(TOK_OREQUAL)>,
  lexer_rule<"xor_eq", simple_return(TOK_XOREQUAL)>, lexer_rule<"not", simple_return(TOK_BANG)>,
  lexer_rule<"not_eq", simple_return(TOK_NOTEQUAL)>,
  lexer_rule<"\\.\\.",
             [](LexerInterface& lexer, auto capture) {
                 lexer.less(1);
                 return TOK_DOT;
             }>,
  lexer_rule<concat<LETTER, ALNUM, "*+">(), simple_return(TOK_NAME)>,
  lexer_rule<concat<"[1-9][0-9]*+", INT_SUFFIX, "?">(), simple_return(TOK_INT_LITERAL)>,
  lexer_rule<concat<"[0][0-7]*+", INT_SUFFIX, "?">(), simple_return(TOK_INT_LITERAL)>,
  lexer_rule<concat<"[0][xX][0-9A-Fa-f]+", INT_SUFFIX, "?">(), simple_return(TOK_INT_LITERAL)>,
  lexer_rule<"[0][xX]",
             [](LexerInterface& lexer, auto capture) {
                 lexer.log(logger::levels::error, "hexadecimal literal with nothing after the 'x'");
                 return TOK_INT_LITERAL;
             }>,
  lexer_rule<
    concat<DIGITS, "\\.", DIGITS, "?(?:[eE]", SIGN, "?", DIGITS, ")?", FLOAT_SUFFIX, "?">(),
    simple_return(TOK_FLOAT_LITERAL)>,
  lexer_rule<concat<DIGITS, "\\.?(?:[eE]", SIGN, "?", DIGITS, ")?", FLOAT_SUFFIX, "?">(),
             simple_return(TOK_FLOAT_LITERAL)>,
  lexer_rule<concat<"\\.", DIGITS, "(?:[eE]", SIGN, "?", DIGITS, ")?", FLOAT_SUFFIX, "?">(),
             simple_return(TOK_FLOAT_LITERAL)>,
  lexer_rule<concat<DIGITS, "\\.", DIGITS, "?[eE]", SIGN, "?">(), error_float_literal>,
  lexer_rule<concat<DIGITS, "\\.", "?[eE]", SIGN, "?">(), error_float_literal>,
  lexer_rule<concat<"\\.", DIGITS, "[eE]", SIGN, "?">(), error_float_literal>,
  lexer_rule<concat<"L?", QUOTE, "((?:", STRCHAR, "|", ESCAPE, ")*+)", QUOTE>(),
             [](LexerInterface& lexer, auto all, auto content) {
                 // lexer.text = content; // no need to store L or quotes in final text.
                 return TOK_STRING_LITERAL;
             }>,
  lexer_rule<concat<"L?", QUOTE, "(?:", STRCHAR, "|", ESCAPE, ")*+", EOL>(),
             [](LexerInterface& lexer, auto content) {
                 if constexpr (allow_newline_string_lits) {
                     lexer.log(logger::levels::warning,
                               "string literal contains (unescaped) newline character; "
                               "this is allowed for gcc-2 bug compatibility only "
                               "(maybe the final `\"' is missing?)");
                     lexer.set_state(states::BUGGY_STRING_LIT);
                 } else {
                     lexer.log(logger::levels::error, "string literal missing final `\"'");
                 }

                 return TOK_STRING_LITERAL;
             }>,
  lexer_rule<concat<"(?:", STRCHAR, "|", ESCAPE, ")*+", QUOTE>(),
             [](LexerInterface& lexer, auto content) {
                 lexer.set_state(state_initial);
                 return TOK_STRING_LITERAL;
             },
             std::array{states::BUGGY_STRING_LIT}>,
  lexer_rule<concat<"(?:", STRCHAR, "|", ESCAPE, ")*+", EOL>(), simple_return(TOK_STRING_LITERAL),
             std::array{states::BUGGY_STRING_LIT}>,
  lexer_rule<concat<"(?:", STRCHAR, "|", ESCAPE, ")*+", BACKSL, "?">(), error_string_literal,
             std::array{states::BUGGY_STRING_LIT}>,
  lexer_rule<concat<"L?", TICK, "(?:", CCCHAR, "|", ESCAPE, ")*+", TICK>(),
             simple_return(TOK_CHAR_LITERAL)>,
  lexer_rule<concat<"L?", TICK, "(?:", CCCHAR, "|", ESCAPE, ")*+", EOL>(),
             [](LexerInterface& lexer, auto content) {
                 lexer.log(logger::levels::error, "character literal missing final \"'\"");
                 return TOK_CHAR_LITERAL;
             }>,
  lexer_rule<concat<"L?", TICK, "(?:", CCCHAR, "|", ESCAPE, ")*+", BACKSL, "?">(),
             [](LexerInterface& lexer, auto content) {
                 lexer.log(logger::levels::error, "unterminated character literal");
                 lexer.terminate();
             }>,
  lexer_rule<concat<"#(?:line)?", SPTAB, ".*+", NL>()>,
  lexer_rule<concat<"#", PPCHAR, "*+(?:\\\\\r?\n", PPCHAR, "*+)*+", BACKSL, "?">()>,
  lexer_rule<"(?:[ \t\n\f\v\r]|\\\\\r?\n)+">, lexer_rule<concat<"//", NOTNL, "*+">()>,
  lexer_rule<"/\\*.*?\\*/">,
  lexer_rule<"/\\*.*?\\*",
             [](LexerInterface& lexer, auto content) {
                 lexer.log(logger::levels::error, "unterminated /"
                                                  "*...*"
                                                  "/ comment");
                 lexer.terminate();
             }>,
  lexer_rule<".",
             [](LexerInterface& lexer, auto content) {
                 // updLoc();
                 lexer.log(logger::levels::error, "bad character");
             }>

  >;

using lexer_definition
  = lexer<tokens, rules, states, state_definitions, derives_from<logger, terminator>,
          default_actions::eof(tokens::eof),
          [](auto& lexer) { throw std::runtime_error("No matching input found,"); }>;

} // namespace definition

int main(int argc, char const* argv[]) {
    // handle no input file
    if (argc < 2) return 1;

    definition::lexer_definition x{};

    x.push_file(argv[1]);

    tokens token;

    size_t as{0};
    while (true) {
        token = x.lex();
        if (token == tokens::eof || token == tokens::no_match) break;

        std::cout << wise_enum::to_string(token) << ' ' << x.text << '\n';
    }

    return !(token == tokens::eof); // 0 on success.
}
