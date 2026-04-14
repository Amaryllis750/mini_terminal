#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "lexer.h"
#include "../../include/parser.h"

int main(){
    char input[1024];
    fgets(input, 1024, stdin);

    Lexer lexer = {NULL, 0};
    lexer_init(&lexer, input);

    while(lexer.pos < strlen(input)){
        Token token = get_next_token(&lexer);
        printf("token is %s, %i\n", token.value, token.type);
    }

    reset_lexer(&lexer);
}

void parse_input(char *input, Parser *parser, Lexer *lexer){
    lexer_init(lexer, input);

    while(lexer->pos < strlen(input)){
        Token token = get_next_token(lexer);
        printf("token is %s, %i", token.value, token.type);
    }
}

void reset_parser(Parser *parser){
    parser->mode = PS1;
    parser->root_node = NULL;
}