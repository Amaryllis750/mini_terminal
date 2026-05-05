#ifndef PARSER_HEADER_H
#define PARSER_HEADER_H

#include "lexer.h"
#include "nodes.h"

typedef enum {
    PS1, 
    PS2
} ShellMode;

typedef struct{
    ShellMode mode;
    char *root_node;    // this would be changed to some custom data structure, an abstract syntax tree
    SequencedCommand *root;
} Parser;

typedef struct Node{
    struct Node *parent;
    struct Node *child_one;
    struct Node *child_two;
} Node;

typedef struct LeafNode{
    Token *token;
    Node *parent;
} LeafNode;

void parse_input(char *input, Parser *parser, Lexer *lexer);

void reset_parser(Parser *parser);

#endif