#include "lexer.h"
#include "rule.h"
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

constexpr ctll::fixed_string NL = "\n";
constexpr ctll::fixed_string NOTNL = "[^\r\n]";
constexpr ctll::fixed_string ANY = concat<"(?:", NOTNL, "|", NL, ")">();
constexpr ctll::fixed_string BACKSL = "\\\\"; // escape the first with the second.
constexpr ctll::fixed_string BOL = "^";
constexpr ctll::fixed_string EOL = "\n";
constexpr ctll::fixed_string LETTER = "[A-Za-z_$]";
constexpr ctll::fixed_string ALNUM = "[A-Za-z_$0-9]";
constexpr ctll::fixed_string DIGIT = "[0-9]";
constexpr ctll::fixed_string HEXDIGIT = "[0-9A-Fa-f]";
constexpr ctll::fixed_string DIGITS = concat<"(?:", DIGIT, "+)">();
constexpr ctll::fixed_string HEXDIGITS = concat<"(?:", HEXDIGIT, "+)">();
constexpr ctll::fixed_string SIGN = concat<"(?:\\+|-)">();
constexpr ctll::fixed_string ELL_SUFFIX = "(?:[lL][lL]?)";
constexpr ctll::fixed_string INT_SUFFIX
  = concat<"(?:[uU]", ELL_SUFFIX, "?|", ELL_SUFFIX, "[uU]?)">();
constexpr ctll::fixed_string FLOAT_SUFFIX = "[flFL]";
constexpr ctll::fixed_string STRCHAR = concat<"[^\"\n", BACKSL, "]">();
constexpr ctll::fixed_string ESCAPE = concat<"(?:", BACKSL, ANY, ")">();
constexpr ctll::fixed_string QUOTE = "[\"]";
constexpr ctll::fixed_string CCCHAR = concat<"[^\'\n", BACKSL, "]">();
constexpr ctll::fixed_string TICK = "[\']";
constexpr ctll::fixed_string SPTAB = "[ \t]";
constexpr ctll::fixed_string PPCHAR = concat<"(?:[^", BACKSL, "\n]|", BACKSL, NOTNL, ")">();

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
  rule<"asm", simple_return(TOK_ASM)>, rule<"auto", simple_return(TOK_AUTO)>,
  rule<"break", simple_return(TOK_BREAK)>, rule<"bool", simple_return(TOK_BOOL)>,
  rule<"case", simple_return(TOK_CASE)>, rule<"catch", simple_return(TOK_CATCH)>,
  rule<"cdecl", simple_return(TOK_CDECL)>, rule<"char", simple_return(TOK_CHAR)>,
  rule<"class", simple_return(TOK_CLASS)>, rule<"const", simple_return(TOK_CONST)>,
  rule<"const_cast", simple_return(TOK_CONST_CAST)>, rule<"continue", simple_return(TOK_CONTINUE)>,
  rule<"default", simple_return(TOK_DEFAULT)>, rule<"delete", simple_return(TOK_DELETE)>,
  rule<"do", simple_return(TOK_DO)>, rule<"double", simple_return(TOK_DOUBLE)>,
  rule<"dynamic_cast", simple_return(TOK_DYNAMIC_CAST)>, rule<"else", simple_return(TOK_ELSE)>,
  rule<"enum", simple_return(TOK_ENUM)>, rule<"explicit", simple_return(TOK_EXPLICIT)>,
  rule<"export", simple_return(TOK_EXPORT)>, rule<"extern", simple_return(TOK_EXTERN)>,
  rule<"false", simple_return(TOK_FALSE)>, rule<"float", simple_return(TOK_FLOAT)>,
  rule<"for", simple_return(TOK_FOR)>, rule<"friend", simple_return(TOK_FRIEND)>,
  rule<"goto", simple_return(TOK_GOTO)>, rule<"if", simple_return(TOK_IF)>,
  rule<"inline", simple_return(TOK_INLINE)>, rule<"int", simple_return(TOK_INT)>,
  rule<"long", simple_return(TOK_LONG)>, rule<"mutable", simple_return(TOK_MUTABLE)>,
  rule<"namespace", simple_return(TOK_NAMESPACE)>, rule<"new", simple_return(TOK_NEW)>,
  rule<"operator", simple_return(TOK_OPERATOR)>, rule<"pascal", simple_return(TOK_PASCAL)>,
  rule<"private", simple_return(TOK_PRIVATE)>, rule<"protected", simple_return(TOK_PROTECTED)>,
  rule<"public", simple_return(TOK_PUBLIC)>, rule<"register", simple_return(TOK_REGISTER)>,
  rule<"reinterpret_cast", simple_return(TOK_REINTERPRET_CAST)>,
  rule<"return", simple_return(TOK_RETURN)>, rule<"short", simple_return(TOK_SHORT)>,
  rule<"signed", simple_return(TOK_SIGNED)>, rule<"sizeof", simple_return(TOK_SIZEOF)>,
  rule<"static", simple_return(TOK_STATIC)>, rule<"static_cast", simple_return(TOK_STATIC_CAST)>,
  rule<"struct", simple_return(TOK_STRUCT)>, rule<"switch", simple_return(TOK_SWITCH)>,
  rule<"template", simple_return(TOK_TEMPLATE)>, rule<"this", simple_return(TOK_THIS)>,
  rule<"throw", simple_return(TOK_THROW)>, rule<"true", simple_return(TOK_TRUE)>,
  rule<"try", simple_return(TOK_TRY)>, rule<"typedef", simple_return(TOK_TYPEDEF)>,
  rule<"typeid", simple_return(TOK_TYPEID)>, rule<"typename", simple_return(TOK_TYPENAME)>,
  rule<"union", simple_return(TOK_UNION)>, rule<"unsigned", simple_return(TOK_UNSIGNED)>,
  rule<"using", simple_return(TOK_USING)>, rule<"virtual", simple_return(TOK_VIRTUAL)>,
  rule<"void", simple_return(TOK_VOID)>, rule<"volatile", simple_return(TOK_VOLATILE)>,
  rule<"wchar_t", simple_return(TOK_WCHAR_T)>, rule<"while", simple_return(TOK_WHILE)>,
  rule<"\\(", simple_return(TOK_LPAREN)>, rule<"\\)", simple_return(TOK_RPAREN)>,
  rule<"\\[", simple_return(TOK_LBRACKET)>, rule<"\\]", simple_return(TOK_RBRACKET)>,
  rule<"->", simple_return(TOK_ARROW)>, rule<"::", simple_return(TOK_COLONCOLON)>,
  rule<"\\.", simple_return(TOK_DOT)>, rule<"!", simple_return(TOK_BANG)>,
  rule<"~", simple_return(TOK_TILDE)>, rule<"\\+", simple_return(TOK_PLUS)>,
  rule<"-", simple_return(TOK_MINUS)>, rule<"\\+\\+", simple_return(TOK_PLUSPLUS)>,
  rule<"--", simple_return(TOK_MINUSMINUS)>, rule<"&", simple_return(TOK_AND)>,
  rule<"\\*", simple_return(TOK_STAR)>, rule<"\\.\\*", simple_return(TOK_DOTSTAR)>,
  rule<"->\\*", simple_return(TOK_ARROWSTAR)>, rule<"/", simple_return(TOK_SLASH)>,
  rule<"%", simple_return(TOK_PERCENT)>, rule<"<<", simple_return(TOK_LEFTSHIFT)>,
  rule<">>", simple_return(TOK_RIGHTSHIFT)>, rule<"<", simple_return(TOK_LESSTHAN)>,
  rule<"<=", simple_return(TOK_LESSEQ)>, rule<">", simple_return(TOK_GREATERTHAN)>,
  rule<">=", simple_return(TOK_GREATEREQ)>, rule<"==", simple_return(TOK_EQUALEQUAL)>,
  rule<"!=", simple_return(TOK_NOTEQUAL)>, rule<"\\^", simple_return(TOK_XOR)>,
  rule<"\\|", simple_return(TOK_OR)>, rule<"&&", simple_return(TOK_ANDAND)>,
  rule<"\\|\\|", simple_return(TOK_OROR)>, rule<"\\?", simple_return(TOK_QUESTION)>,
  rule<":", simple_return(TOK_COLON)>, rule<"=", simple_return(TOK_EQUAL)>,
  rule<"\\*=", simple_return(TOK_STAREQUAL)>, rule<"/=", simple_return(TOK_SLASHEQUAL)>,
  rule<"%=", simple_return(TOK_PERCENTEQUAL)>, rule<"\\+=", simple_return(TOK_PLUSEQUAL)>,
  rule<"-=", simple_return(TOK_MINUSEQUAL)>, rule<"&=", simple_return(TOK_ANDEQUAL)>,
  rule<"\\^=", simple_return(TOK_XOREQUAL)>, rule<"\\|=", simple_return(TOK_OREQUAL)>,
  rule<"<<=", simple_return(TOK_LEFTSHIFTEQUAL)>, rule<">>=", simple_return(TOK_RIGHTSHIFTEQUAL)>,
  rule<",", simple_return(TOK_COMMA)>, rule<"\\.\\.\\.", simple_return(TOK_ELLIPSIS)>,
  rule<";", simple_return(TOK_SEMICOLON)>, rule<"\\{", simple_return(TOK_LBRACE)>,
  rule<"\\}", simple_return(TOK_RBRACE)>, rule<"<%", simple_return(TOK_LBRACE)>,
  rule<"%>", simple_return(TOK_RBRACE)>, rule<"<:", simple_return(TOK_LBRACKET)>,
  rule<":>", simple_return(TOK_RBRACKET)>, rule<"and", simple_return(TOK_ANDAND)>,
  rule<"bitor", simple_return(TOK_OR)>, rule<"or", simple_return(TOK_OROR)>,
  rule<"xor", simple_return(TOK_XOR)>, rule<"compl", simple_return(TOK_TILDE)>,
  rule<"bitand", simple_return(TOK_AND)>, rule<"and_eq", simple_return(TOK_ANDEQUAL)>,
  rule<"or_eq", simple_return(TOK_OREQUAL)>, rule<"xor_eq", simple_return(TOK_XOREQUAL)>,
  rule<"not", simple_return(TOK_BANG)>, rule<"not_eq", simple_return(TOK_NOTEQUAL)>,
  rule<"\\.\\.",
       [](LexerInterface& lexer, auto capture) {
           lexer.less(1);
           return TOK_DOT;
       }>,
  rule<concat<LETTER, ALNUM, "*+">(), simple_return(TOK_NAME)>,
  rule<concat<"[1-9][0-9]*+", INT_SUFFIX, "?">(), simple_return(TOK_INT_LITERAL)>,
  rule<concat<"[0][0-7]*+", INT_SUFFIX, "?">(), simple_return(TOK_INT_LITERAL)>,
  rule<concat<"[0][xX][0-9A-Fa-f]+", INT_SUFFIX, "?">(), simple_return(TOK_INT_LITERAL)>,
  rule<"[0][xX]",
       [](LexerInterface& lexer, auto capture) {
           lexer.log(logger::levels::error, "hexadecimal literal with nothing after the 'x'");
           return TOK_INT_LITERAL;
       }>,
  rule<concat<DIGITS, "\\.", DIGITS, "?(?:[eE]", SIGN, "?", DIGITS, ")?", FLOAT_SUFFIX, "?">(),
       simple_return(TOK_FLOAT_LITERAL)>,
  rule<concat<DIGITS, "\\.?(?:[eE]", SIGN, "?", DIGITS, ")?", FLOAT_SUFFIX, "?">(),
       simple_return(TOK_FLOAT_LITERAL)>,
  rule<concat<"\\.", DIGITS, "(?:[eE]", SIGN, "?", DIGITS, ")?", FLOAT_SUFFIX, "?">(),
       simple_return(TOK_FLOAT_LITERAL)>,
  rule<concat<DIGITS, "\\.", DIGITS, "?[eE]", SIGN, "?">(), error_float_literal>,
  rule<concat<DIGITS, "\\.", "?[eE]", SIGN, "?">(), error_float_literal>,
  rule<concat<"\\.", DIGITS, "[eE]", SIGN, "?">(), error_float_literal>,
  rule<concat<"L?", QUOTE, "((?:", STRCHAR, "|", ESCAPE, ")*+)", QUOTE>(),
       [](LexerInterface& lexer, auto all, auto content) {
           // lexer.text = content; // no need to store L or quotes in final text.
           return TOK_STRING_LITERAL;
       }>,
  rule<concat<"L?", QUOTE, "(?:", STRCHAR, "|", ESCAPE, ")*+", EOL>(),
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
  rule<concat<"(?:", STRCHAR, "|", ESCAPE, ")*+", QUOTE>(),
       [](LexerInterface& lexer, auto content) {
           lexer.set_state(state_initial);
           return TOK_STRING_LITERAL;
       },
       std::array{states::BUGGY_STRING_LIT}>,
  rule<concat<"(?:", STRCHAR, "|", ESCAPE, ")*+", EOL>(), simple_return(TOK_STRING_LITERAL),
       std::array{states::BUGGY_STRING_LIT}>,
  rule<concat<"(?:", STRCHAR, "|", ESCAPE, ")*+", BACKSL, "?">(), error_string_literal,
       std::array{states::BUGGY_STRING_LIT}>,
  rule<concat<"L?", TICK, "(?:", CCCHAR, "|", ESCAPE, ")*+", TICK>(),
       simple_return(TOK_CHAR_LITERAL)>,
  rule<concat<"L?", TICK, "(?:", CCCHAR, "|", ESCAPE, ")*+", EOL>(),
       [](LexerInterface& lexer, auto content) {
           lexer.log(logger::levels::error, "character literal missing final \"'\"");
           return TOK_CHAR_LITERAL;
       }>,
  rule<concat<"L?", TICK, "(?:", CCCHAR, "|", ESCAPE, ")*+", BACKSL, "?">(),
       [](LexerInterface& lexer, auto content) {
           lexer.log(logger::levels::error, "unterminated character literal");
           lexer.terminate();
       }>,
  rule<concat<"#(?:line)?", SPTAB, ".*+", NL>()>,
  rule<concat<"#", PPCHAR, "*+(?:\\\\\r?\n", PPCHAR, "*+)*+", BACKSL, "?">()>,
  rule<"(?:[ \t\n\f\v\r]|\\\\\r?\n)+">, rule<concat<"//", NOTNL, "*+">()>, rule<"/\\*.*?\\*/">,
  rule<"/\\*.*?\\*",
       [](LexerInterface& lexer, auto content) {
           lexer.log(logger::levels::error, "unterminated /"
                                            "*...*"
                                            "/ comment");
           lexer.terminate();
       }>,
  rule<".",
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
