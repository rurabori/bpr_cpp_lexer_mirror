lexer grammar antlr_cpp_lexer;

fragment NL : '\n';
fragment NOTNL : ~('\n');
fragment ANY : .;
fragment BACKSL : '\\';
fragment EOL : NL;
fragment LETTER : [A-Za-z_$];
fragment ALNUM : [A-Za-z_0-9$];
fragment DIGIT : [0-9];
fragment HEXDIGIT : [0-9A-Fa-f];
fragment DIGITS : ( DIGIT+ );
fragment HEXDIGITS : ( HEXDIGIT+ );
fragment SIGN : ( '+' | '-' );
fragment ELL_SUFFIX : [lL]([lL]?);
fragment INT_SUFFIX : ([uU] ELL_SUFFIX ?| ELL_SUFFIX [uU] ?);
fragment FLOAT_SUFFIX : [flFL];
fragment QUOTE : '"';
fragment TICK : '\'';
fragment STRCHAR : ~( '"' | '\n' | '\\' );
fragment ESCAPE : ( BACKSL ANY );
fragment CCCHAR : ~( '\'' | '\n' | '\\' );
fragment SPTAB : [ \t];
fragment PPCHAR : ( ~( '\\' | '\n' ) | BACKSL NOTNL);

TOK_ASM : 'asm';
TOK_AUTO : 'auto';
TOK_BREAK : 'break';
TOK_BOOL : 'bool';
TOK_CASE : 'case';
TOK_CATCH : 'catch';
TOK_CDECL : 'cdecl';
TOK_CHAR : 'char';
TOK_CLASS : 'class';
TOK_CONST : 'const';
TOK_CONST_CAST : 'const_cast';
TOK_CONTINUE : 'continue';
TOK_DEFAULT : 'default';
TOK_DELETE : 'delete';
TOK_DO : 'do';
TOK_DOUBLE : 'double';
TOK_DYNAMIC_CAST : 'dynamic_cast';
TOK_ELSE : 'else';
TOK_ENUM : 'enum';
TOK_EXPLICIT : 'explicit';
TOK_EXPORT : 'export';
TOK_EXTERN : 'extern';
TOK_FALSE : 'false';
TOK_FLOAT : 'float';
TOK_FOR : 'for';
TOK_FRIEND : 'friend';
TOK_GOTO : 'goto';
TOK_IF : 'if';
TOK_INLINE : 'inline';
TOK_INT : 'int';
TOK_LONG : 'long';
TOK_MUTABLE : 'mutable';
TOK_NAMESPACE : 'namespace';
TOK_NEW : 'new';
TOK_OPERATOR : 'operator';
TOK_PASCAL : 'pascal';
TOK_PRIVATE : 'private';
TOK_PROTECTED : 'protected';
TOK_PUBLIC : 'public';
TOK_REGISTER : 'register';
TOK_REINTERPRET_CAST : 'reinterpret_cast';
TOK_RETURN : 'return';
TOK_SHORT : 'short';
TOK_SIGNED : 'signed';
TOK_SIZEOF : 'sizeof';
TOK_STATIC : 'static';
TOK_STATIC_CAST : 'static_cast';
TOK_STRUCT : 'struct';
TOK_SWITCH : 'switch';
TOK_TEMPLATE : 'template';
TOK_THIS : 'this';
TOK_THROW : 'throw';
TOK_TRUE : 'true';
TOK_TRY : 'try';
TOK_TYPEDEF : 'typedef';
TOK_TYPEID : 'typeid';
TOK_TYPENAME : 'typename';
TOK_UNION : 'union';
TOK_UNSIGNED : 'unsigned';
TOK_USING : 'using';
TOK_VIRTUAL : 'virtual';
TOK_VOID : 'void';
TOK_VOLATILE : 'volatile';
TOK_WCHAR_T : 'wchar_t';
TOK_WHILE : 'while';

TOK_LPAREN : '(';
TOK_RPAREN : ')';
TOK_LBRACKET    : '['
                | '<:';
TOK_RBRACKET    : ']'
                | ':>';
TOK_ARROW : '->';
TOK_COLONCOLON : '::';
TOK_DOT : '.';
TOK_BANG    : '!'
            | 'not';
TOK_TILDE   : '~'
            | 'compl';
TOK_PLUS : '+';
TOK_MINUS : '-';
TOK_PLUSPLUS : '++';
TOK_MINUSMINUS : '--';
TOK_AND : '&'
        | 'bitand';
TOK_STAR : '*';
TOK_DOTSTAR : '.*';
TOK_ARROWSTAR : '->*';
TOK_SLASH : '/';
TOK_PERCENT : '%';
TOK_LEFTSHIFT : '<<';
TOK_RIGHTSHIFT : '>>';
TOK_LESSTHAN : '<';
TOK_LESSEQ : '<=';
TOK_GREATERTHAN : '>';
TOK_GREATEREQ : '>=';
TOK_EQUALEQUAL : '==';
TOK_NOTEQUAL    : '!='
                | 'not_eq';
TOK_XOR : '^'
        | 'xor';
TOK_OR  : '|'
        | 'bitor';
TOK_ANDAND  : '&&'
            | 'and';
TOK_OROR    : '||'
            | 'or';
TOK_QUESTION : '?';
TOK_COLON : ':';
TOK_EQUAL : '=';
TOK_STAREQUAL : '*=';
TOK_SLASHEQUAL : '/=';
TOK_PERCENTEQUAL : '%=';
TOK_PLUSEQUAL : '+=';
TOK_MINUSEQUAL : '-=';
TOK_ANDEQUAL    : '&='
                | 'and_eq';
TOK_XOREQUAL    : '^='
                | 'xor_eq';
TOK_OREQUAL : '|='
            | 'or_eq';
TOK_LEFTSHIFTEQUAL : '<<=';
TOK_RIGHTSHIFTEQUAL : '>>=';
TOK_COMMA : ',';
TOK_ELLIPSIS : '...';
TOK_SEMICOLON : ';';
TOK_LBRACE  : '{'
            | '<%';
TOK_RBRACE  : '}'
            | '%>';

TOK_NAME : LETTER ALNUM*;
  
TOK_INT_LITERAL     : [1-9][0-9]* INT_SUFFIX?
                    | [0][0-7]* INT_SUFFIX?
                    | [0][xX][0-9A-Fa-f]+ INT_SUFFIX?;

ERR_INT_LITERAL : [0][xX] {
  std::cerr << "hexadecimal literal with nothing after the 'x'" << '\n';
} -> skip;

  
TOK_FLOAT_LITERAL   : DIGITS '.' DIGITS ?([eE] SIGN ? DIGITS )? FLOAT_SUFFIX ?   
                    | DIGITS '.'?([eE] SIGN ? DIGITS )? FLOAT_SUFFIX ?
                    |'.' DIGITS ([eE] SIGN ? DIGITS )? FLOAT_SUFFIX ?;

ERR_FLOAT_LITERAL   : DIGITS '.' DIGITS ?[eE] SIGN ? {
                        std::cerr << "floating literal with no digits after the 'e'" << '\n';
                        skip();
                    }
                    | DIGITS '.'?[eE] SIGN ? {
                        std::cerr << "floating literal with no digits after the 'e'" << '\n';
                        skip();
                    }
                    | '.' DIGITS [eE] SIGN ? {
                       std::cerr << "floating literal with no digits after the 'e'" << '\n';
                        skip();
                    };

  
TOK_STRING_LITERAL : 'L'? QUOTE ( STRCHAR | ESCAPE )* QUOTE;
  
ERR_STRING_LITERAL : 'L'? QUOTE ( STRCHAR | ESCAPE )* BACKSL ? {
  throw std::runtime_error("unterminated string literal");
};

TOK_CHAR_LITERAL    : 'L' ? TICK ( CCCHAR | ESCAPE )* TICK
                    | 'L' ? TICK ( CCCHAR | ESCAPE )* EOL {
                    std::cerr << "character literal missing final \"'\"" << '\n';
                };

  
ERR_CHAR_LITERAL    : 'L'? TICK ( CCCHAR | ESCAPE )* BACKSL ?  {
                        throw std::runtime_error("unterminated char literal");
                    };
PREPROCESSOR_COMMAND    : '#' 'line' SPTAB .*? NL { skip(); }
                        | '#' PPCHAR *( BACKSL '\r' ?  NL  PPCHAR *)* BACKSL ? { skip(); };

WHITESPACE : ( [ \t\n\f\r] | BACKSL '\r' ? NL )+ -> skip;

  
  
SINGLE_LINE_COMMENT : '//' NOTNL * -> skip;
COMMENT : '/*' .*? '*/' { skip(); }
        | '/*' .*? '*' {
            std::runtime_error("unterminated /""*...*""/ comment");
            skip();
        };
