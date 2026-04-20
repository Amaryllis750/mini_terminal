#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "lexer.h"
#include "parser.h"

void parse_input(char *input, Parser *parser, Lexer *lexer){
    while(lexer->pos < strlen(input)){
        Token token = get_next_token(lexer);
        if(token.type == TOK_NEWLINE && lexer->pos == strlen(input)){
            printf("new line\n");
        }
        if(token.type == INCOMPLETE_TOKEN){
            printf("I have detected an incomplete token\n");
            parser->mode = PS2;
            return ;
        }
        else printf("token is %s, %i\n", token.value, token.type);
    }
    printf("While leaving the parser, We are in %s mode\n", parser->mode == PS1 ? "PS1" : "PS2");
    parser->mode = PS1; // if the parser has left the loop, it has gathered all tokens
}

void reset_parser(Parser *parser){
    parser->mode = PS1;
    parser->root_node = NULL;
}