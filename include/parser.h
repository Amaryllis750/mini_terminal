#ifndef PARSER_HEADER_H
#define PARSER_HEADER_H

#include "lexer.h"

typedef enum {
    PS1, 
    PS2
} ShellMode;

typedef struct{
    ShellMode mode;
    char *root_node;    // this would be changed to some custom data structure, an abstract syntax tree
} Parser;

void parse_input(char *input, Parser *parser, Lexer *lexer);

void reset_parser(Parser *parser);

#endif