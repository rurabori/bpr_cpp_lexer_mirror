# CTLE - Compile Time LExer

A compile-time generated lexical analyzer.

## Example:
```c++
enum tokens {
  tok_int,
  tok_double
};

using rules = ctll::list<
  // return value to caller
  ctle::rule<"int", [](auto&&...){ return tokens::tok_int; }>,
  // don't return to caller
  ctle::rule<"[ \t\n\f\v\r]+", [](auto& lexer, auto lexeme) { count_whitespace(lexeme); } >
  ...
>;

ctle::lexer<tokens, rules> lexer;
ctle::basic_file<char> input{"main.cpp"};
lexer.set_input(input);
while (true) {
  auto [token, lexeme] = lexer.lex();
}
...
```

## Compiling
CTLE makes use of cnttp so it requires C++20, currently only GCC9 supports all the features needed.

## Regular Expressions
CTLE uses CTRE (https://github.com/hanickadot/compile-time-regular-expressions) as its regex driver. It supports captures, which changes
the signature of an action.
```c++
ctle::rule<"no_captures", [](auto& lexer, auto lexeme) {}>
ctle::rule<"one_(capture)", [](auto& lexer, auto lexeme, auto capture1) {}>
```
etc.
## Retutning values to caller of lex()
The callable provided as an action must have one of the three return types:
```c++
void // does not return to caller of lex()
ReturnT // does return to the caller of lex()
std::optional<ReturnT> // returns only if optional has value
```
Where ReturnT is the first template parameter passed to ctle::lexer.
