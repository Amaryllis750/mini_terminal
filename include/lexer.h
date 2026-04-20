#ifndef LEXER_HEADER_H
#define LEXER_HEADER_H

typedef enum{
    TOK_DOUBLE_QUOTED_WORD,
    TOK_BARE_WORD,
    TOK_SINGLE_QUOTED_WORD,
    TOK_REDIRECT_STDIN, // <
    TOK_REDIRECT_STDOUT, // >
    TOK_REDIRECT_STDERR, // 2>
    TOK_APPEND, // >>
    TOK_SEQUENCE,  // ;
    TOK_PIPE, // |
    TOK_EOF, // end of line
    INCOMPLETE_TOKEN,
    TOK_NEWLINE,
} TokenType;

typedef struct {
    TokenType type;
    char *value;    // everything that enters the tokenizer is treated as a string
    int position;
} Token;

typedef struct{
    char *input;
    int pos;
    int start;
    char *err;
} Lexer;

void lexer_init(Lexer *l, char *input);
void reset_lexer(Lexer *l);

Token get_next_token(Lexer *l);

#endif