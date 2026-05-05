#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "lexer.h"
#include "parser.h"

void parse_input(char *input, Parser *parser, Lexer *lexer){
    while(lexer->pos < strlen(input)){
        Token token = get_next_token(lexer);
        if(token.type == INCOMPLETE_TOKEN){
            parser->mode = PS2;
            return ;
        }
    }
    parser->mode = PS1; // if the parser has left the loop, it has gathered all tokens
}


void reset_parser(Parser *parser){
    parser->mode = PS1;
    parser->root_node = NULL;
}