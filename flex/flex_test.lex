/* cc.lex            see license.txt for copyright and terms of use
 * flex description of scanner for C and C++ souce
 */

/* ----------------------- C definitions ---------------------- */
%{

#include "tokens.h"
#include <fstream>
#include <iostream>

// this works around a problem with cygwin & fileno
#define YY_NEVER_INTERACTIVE 1
%}


/* -------------------- flex options ------------------ */
/* no wrapping is needed; setting this means we don't have to link with libfl.a */
%option noyywrap

/* don't use the default-echo rules */
%option nodefault

/* I don't call unput */
%option nounput

/* use the "fast" algorithm with no table compression */
%option full

/* utilize character equivalence classes */
%option ecs

/* the scanner is never interactive */
%option never-interactive

/* output file name */
  /* dsw: Arg!  Don't do this, since it is not overrideable from the
     command line with -o, which I consider to be a flex bug. */
  /* sm: fair enough; agreed */
  /* %option outfile="lexer.yy.cc" */

/* start conditions */
%x BUGGY_STRING_LIT

/* ------------------- definitions -------------------- */
/* newline */
NL            "\n"

/* anything but newline */
NOTNL         .

/* any of 256 source characters */
ANY           ({NOTNL}|{NL})

/* backslash */
BACKSL        "\\"

/* beginnging of line (must be start of a pattern) */
BOL           ^

/* end of line (would like EOF to qualify also, but flex doesn't allow it */
EOL           {NL}

/* letter or underscore */
LETTER        [A-Za-z_]

/* letter or underscore or digit */
ALNUM         [A-Za-z_0-9]

/* decimal digit */
DIGIT         [0-9]
HEXDIGIT      [0-9A-Fa-f]

/* sequence of decimal digits */
DIGITS        ({DIGIT}+)
/* sequence of hex digits */
HEXDIGITS     ({HEXDIGIT}+)

/* sign of a number */
SIGN          ("+"|"-")

/* integer suffix */
/* added 'LL' option for gcc/c99 long long compatibility */
ELL_SUFFIX    [lL]([lL]?)
INT_SUFFIX    ([uU]{ELL_SUFFIX}?|{ELL_SUFFIX}[uU]?)

/* floating-point suffix letter */
FLOAT_SUFFIX  [flFL]

/* normal string character: any but quote, newline, or backslash */
STRCHAR       [^\"\n\\]

/* (start of) an escape sequence */
ESCAPE        ({BACKSL}{ANY})

/* double quote */
QUOTE         [\"]

/* normal character literal character: any but single-quote, newline, or backslash */
CCCHAR        [^\'\n\\]

/* single quote */
TICK          [\']

/* space or tab */
SPTAB         [ \t]

/* preprocessor "character" -- any but escaped newline */
PPCHAR        ([^\\\n]|{BACKSL}{NOTNL})


/* ------------- token definition rules --------------- */
%%

  /* this comment is replaced, by an external script, with whatever
   * additional rules a language extension author desires */
  /* EXTENSION RULES GO HERE */


  /* operators, punctuators and keywords: tokens with one spelling */
"asm"              return TOK_ASM;
"auto"             return TOK_AUTO;
"break"            return TOK_BREAK;
"bool"             return TOK_BOOL;
"case"             return TOK_CASE;
"catch"            return TOK_CATCH;
"cdecl"            return TOK_CDECL;
"char"             return TOK_CHAR;
"class"            return TOK_CLASS;
"const"            return TOK_CONST;
"const_cast"       return TOK_CONST_CAST;
"continue"         return TOK_CONTINUE;
"default"          return TOK_DEFAULT;
"delete"           return TOK_DELETE;
"do"               return TOK_DO;
"double"           return TOK_DOUBLE;
"dynamic_cast"     return TOK_DYNAMIC_CAST;
"else"             return TOK_ELSE;
"enum"             return TOK_ENUM;
"explicit"         return TOK_EXPLICIT;
"export"           return TOK_EXPORT;
"extern"           return TOK_EXTERN;
"false"            return TOK_FALSE;
"float"            return TOK_FLOAT;
"for"              return TOK_FOR;
"friend"           return TOK_FRIEND;
"goto"             return TOK_GOTO;
"if"               return TOK_IF;
"inline"           return TOK_INLINE;
"int"              return TOK_INT;
"long"             return TOK_LONG;
"mutable"          return TOK_MUTABLE;
"namespace"        return TOK_NAMESPACE;
"new"              return TOK_NEW;
"operator"         return TOK_OPERATOR;
"pascal"           return TOK_PASCAL;
"private"          return TOK_PRIVATE;
"protected"        return TOK_PROTECTED;
"public"           return TOK_PUBLIC;
"register"         return TOK_REGISTER;
"reinterpret_cast" return TOK_REINTERPRET_CAST;
"return"           return TOK_RETURN;
"short"            return TOK_SHORT;
"signed"           return TOK_SIGNED;
"sizeof"           return TOK_SIZEOF;
"static"           return TOK_STATIC;
"static_cast"      return TOK_STATIC_CAST;
"struct"           return TOK_STRUCT;
"switch"           return TOK_SWITCH;
"template"         return TOK_TEMPLATE;
"this"             return TOK_THIS;
"throw"            return TOK_THROW;
"true"             return TOK_TRUE;
"try"              return TOK_TRY;
"typedef"          return TOK_TYPEDEF;
"typeid"           return TOK_TYPEID;
"typename"         return TOK_TYPENAME;
"union"            return TOK_UNION;
"unsigned"         return TOK_UNSIGNED;
"using"            return TOK_USING;
"virtual"          return TOK_VIRTUAL;
"void"             return TOK_VOID;
"volatile"         return TOK_VOLATILE;
"wchar_t"          return TOK_WCHAR_T;
"while"            return TOK_WHILE;

"("                return TOK_LPAREN;
")"                return TOK_RPAREN;
"["                return TOK_LBRACKET;
"]"                return TOK_RBRACKET;
"->"               return TOK_ARROW;
"::"               return TOK_COLONCOLON;
"."                return TOK_DOT;
"!"                return TOK_BANG;
"~"                return TOK_TILDE;
"+"                return TOK_PLUS;
"-"                return TOK_MINUS;
"++"               return TOK_PLUSPLUS;
"--"               return TOK_MINUSMINUS;
"&"                return TOK_AND;
"*"                return TOK_STAR;
".*"               return TOK_DOTSTAR;
"->*"              return TOK_ARROWSTAR;
"/"                return TOK_SLASH;
"%"                return TOK_PERCENT;
"<<"               return TOK_LEFTSHIFT;
">>"               return TOK_RIGHTSHIFT;
"<"                return TOK_LESSTHAN;
"<="               return TOK_LESSEQ;
">"                return TOK_GREATERTHAN;
">="               return TOK_GREATEREQ;
"=="               return TOK_EQUALEQUAL;
"!="               return TOK_NOTEQUAL;
"^"                return TOK_XOR;
"|"                return TOK_OR;
"&&"               return TOK_ANDAND;
"||"               return TOK_OROR;
"?"                return TOK_QUESTION;
":"                return TOK_COLON;
"="                return TOK_EQUAL;
"*="               return TOK_STAREQUAL;
"/="               return TOK_SLASHEQUAL;
"%="               return TOK_PERCENTEQUAL;
"+="               return TOK_PLUSEQUAL;
"-="               return TOK_MINUSEQUAL;
"&="               return TOK_ANDEQUAL;
"^="               return TOK_XOREQUAL;
"|="               return TOK_OREQUAL;
"<<="              return TOK_LEFTSHIFTEQUAL;
">>="              return TOK_RIGHTSHIFTEQUAL;
","                return TOK_COMMA;
"..."              return TOK_ELLIPSIS;
";"                return TOK_SEMICOLON;
"{"                return TOK_LBRACE;
"}"                return TOK_RBRACE;

  /* "alternative tokens" of cppstd 2.5p2 */
"<%"               return TOK_LBRACE;
"%>"               return TOK_RBRACE;
"<:"               return TOK_LBRACKET;
":>"               return TOK_RBRACKET;
  /* "%:" and "%:%:" correspond to "#" and "##", which are only for
   * the preprocessor, so I will ignore them here */
"and"              return TOK_ANDAND;
"bitor"            return TOK_OR;
"or"               return TOK_OROR;
"xor"              return TOK_XOR;
"compl"            return TOK_TILDE;
"bitand"           return TOK_AND;
"and_eq"           return TOK_ANDEQUAL;
"or_eq"            return TOK_OREQUAL;
"xor_eq"           return TOK_XOREQUAL;
"not"              return TOK_BANG;
"not_eq"           return TOK_NOTEQUAL;

  /* this rule is to avoid backing up in the lexer
   * when there are two dots but not three */
".." {
  yyless(1);     /* put back all but 1; this is inexpensive */
  return TOK_DOT;
}

  /* identifier: e.g. foo */
{LETTER}{ALNUM}* {
  return TOK_NAME;
}

  /* integer literal; dec, oct, or hex */
[1-9][0-9]*{INT_SUFFIX}?           |
[0][0-7]*{INT_SUFFIX}?             |
[0][xX][0-9A-Fa-f]+{INT_SUFFIX}?   {
  return TOK_INT_LITERAL;
}

  /* hex literal with nothing after the 'x' */
[0][xX] {
  std::cerr << "hexadecimal literal with nothing after the 'x'" << '\n';
  return TOK_INT_LITERAL;
}

  /* floating literal */
{DIGITS}"."{DIGITS}?([eE]{SIGN}?{DIGITS})?{FLOAT_SUFFIX}?   |
{DIGITS}"."?([eE]{SIGN}?{DIGITS})?{FLOAT_SUFFIX}?	    |
"."{DIGITS}([eE]{SIGN}?{DIGITS})?{FLOAT_SUFFIX}?	    {
  return TOK_FLOAT_LITERAL;
}

  /* floating literal with no digits after the 'e' */
{DIGITS}"."{DIGITS}?[eE]{SIGN}?   |
{DIGITS}"."?[eE]{SIGN}?           |
"."{DIGITS}[eE]{SIGN}?            {
  std::cerr << "floating literal with no digits after the 'e'" << '\n';
  
  /* in recovery rules like this it's best to yield the best-guess
   * token type, instead of some TOK_ERROR, so the parser can still
   * try to make sense of the input; having reported the error is
   * sufficient to ensure that later stages won't try to interpret
   * the lexical text of this token as a floating literal */
  return TOK_FLOAT_LITERAL;
}

  /* string literal */
"L"?{QUOTE}({STRCHAR}|{ESCAPE})*{QUOTE} {
  return TOK_STRING_LITERAL;
}

  /* string literal missing final quote */
"L"?{QUOTE}({STRCHAR}|{ESCAPE})*{EOL}   {
    std::cerr << "string literal contains (unescaped) newline character; "
            "this is allowed for gcc-2 bug compatibility only "
            "(maybe the final `\"' is missing?)" << '\n';
    BEGIN(BUGGY_STRING_LIT);
    return TOK_STRING_LITERAL;
}

  /* unterminated string literal; maximal munch causes us to prefer
   * either of the above two rules when possible; the trailing
   * optional backslash is needed so the scanner won't back up in that
   * case; NOTE: this can only happen if the file ends in the string
   * and there is no newline before the EOF */
"L"?{QUOTE}({STRCHAR}|{ESCAPE})*{BACKSL}? {
  std::cerr << "unterminated string literal" << '\n';
  yyterminate();
}


  /* This scanner reads in a string literal that contains unescaped
   * newlines, to support a gcc-2 bug.  The strategy is to emit a
   * sequence of TOK_STRING_LITERALs, as if the string had been
   * properly broken into multiple literals.  However, these literals
   * aren't consistently surrounded by quotes... */
<BUGGY_STRING_LIT>{
  ({STRCHAR}|{ESCAPE})*{QUOTE} {
    // found the end
    BEGIN(INITIAL);
    return TOK_STRING_LITERAL;
  }
  ({STRCHAR}|{ESCAPE})*{EOL} {
    // another line
    return TOK_STRING_LITERAL;
  }
  <<EOF>> |
  ({STRCHAR}|{ESCAPE})*{BACKSL}? {
    // unterminated (this only matches at EOF)
    std::cerr << "at EOF, unterminated string literal; support for newlines in string "
        "literals is presently turned on, maybe the missing quote should have "
        "been much earlier in the file?" << '\n';
    yyterminate();
  }
}


  /* character literal */
"L"?{TICK}({CCCHAR}|{ESCAPE})*{TICK}   {
  return TOK_CHAR_LITERAL;
}

  /* character literal missing final tick */
"L"?{TICK}({CCCHAR}|{ESCAPE})*{EOL}    {
  std::cerr << "character literal missing final \"'\"" << '\n';
  return TOK_CHAR_LITERAL;       // error recovery
}

  /* unterminated character literal */
"L"?{TICK}({CCCHAR}|{ESCAPE})*{BACKSL}?  {
  std::cerr << "unterminated character literal" << '\n';
  yyterminate();
}


  /* sm: I moved the user-defined qualifier rule into qual_ext.lex
   * in the cc_qual tree */


  /* preprocessor */
  /* technically, if this isn't at the start of a line (possibly after
   * some whitespace), it should be an error.. I'm not sure right now how
   * I want to deal with that (I originally was using '^', but that
   * interacts badly with the whitespace rule) */

  /* #line directive: the word "line" is optional, then a space, and
   * then we accept the rest of the line; 'parseHashLine' will finish
   * parsing the directive */
"#"("line"?){SPTAB}.*{NL} {
}

  /* other preprocessing: ignore it */
  /* trailing optional baskslash to avoid backing up */
"#"{PPCHAR}*({BACKSL}{NL}{PPCHAR}*)*{BACKSL}?   {
  // treat it like whitespace, ignoring it otherwise
}

  /* whitespace */
  /* 10/20/02: added '\r' to accomodate files coming from Windows; this
   * could be seen as part of the mapping from physical source file
   * characters to the basic character set (cppstd 2.1 para 1 phase 1),
   * except that it doesn't happen for chars in string/char literals... */
[ \t\n\f\v\r]+  {
}

  /* C++ comment */
  /* we don't match the \n because that way this works at EOF */
"//"{NOTNL}*    {
}

  /* C comment */
"/""*"([^*]|"*"*[^*/])*"*"+"/"     {
  // the pattern is a little complicated because the naive one,
  //   "/""*"([^*]|"*"[^/])*"*/"
  // fails to match e.g. "/***/" 
}

  /* unterminated C comment */
"/""*"([^*]|"*"*[^*/])*"*"*        {
  std::cerr << "unterminated /""*...*""/ comment" << '\n';
  yyterminate();
}


  /* illegal */
.  {
  std::cerr << "illegal character: `" << yytext[0] << "'\n";
}

<<EOF>> {
  yyterminate();
}
%%



int main(int argc, char const *argv[])
{
    // handle no input file
    if (argc < 2) return 1;

    yyin = fopen(argv[1], "r");
   
    while (tokens token = static_cast<tokens>(yylex()))
        std::cout << wise_enum::to_string(token) << '\n';

    return 0;
}
