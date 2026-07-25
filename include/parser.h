#ifndef PARSER_HEADER_H
#define PARSER_HEADER_H

#include "lexer.h"
#include "nodes.h"

typedef enum {
    PS1, 
    PS2
} ShellMode;

#define SHOULD_ABORT(parser) ((parser)->error != PARSER_OK || (parser)->mode == PS2)


typedef enum {
    PARSER_OK = 0,
    PARSER_ERR_UNKNOWN_FLAG,
    PARSER_ERR_MISSING_VALUE,
    PARSER_ERR_INVALID_VALUE,
    PARSER_ERR_UNEXPECTED_ARG,
    PARSER_ERR_MEMORY,
} ParserError;



typedef struct{
    ShellMode mode;
    SequencedCommand *root;
    Token current_token;

    // Error handling
    ParserError error;
    Token  *err_token;
    char *err_msg;

} Parser;

void parse_input(char *input, Parser *parser, Lexer *lexer);


SequencedCommand sequence_command(Parser *p, Lexer *l);
PipedCommand piped_command(Parser *p, Lexer *l);
RedirectedCommand redirected_command(Parser *p, Lexer *l);
Command get_command(Parser *p, Lexer *l);


void reset_parser(Parser *parser);
void init_parser(Parser *p, Lexer *l);
void handle_incomplete_token(Parser *p);


Token peek(Parser *p);
Token consume(Parser *p, Lexer *l);

Token strip_quotes(Token t);
#endif