#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "lexer.h"
#include "parser.h"

void parse_input(char *input, Parser *parser, Lexer *lexer){
    lexer_init(lexer, input);

    while(lexer->pos < strlen(input)){
        Token token = get_next_token(lexer);
        if(token.type == TOK_NEWLINE && lexer->pos == strlen(input)){
            printf("new line\n");
        }
        else printf("token is %s, %i\n", token.value, token.type);
    }
}

void reset_parser(Parser *parser){
    parser->mode = PS1;
    parser->root_node = NULL;
}