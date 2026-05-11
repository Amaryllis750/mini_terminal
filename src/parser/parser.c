#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "nodes.h"

Token current_token = {};

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


/**
 * Each of the recursive functions will return a tree or subtree. 
 * For the case of the sequence command, the entire tree will be returned since it is the highest node
 * For the other nodes, they would each return subtrees
 */

SequencedCommand sequence_command(Lexer *lexer){
    Token token = get_next_token(lexer);

    // pass the token into the piped command

}

PipedCommand piped_command(Lexer *lexer){
    // pass the token to the redirected command
    RedirectedCommand r_command = redirected_command(lexer);
    
}

RedirectedCommand redirected_command(Lexer *lexer){
    // pass the token to the command
    Command command = get_command(lexer);
}

Command get_command(Lexer *lexer){
    // pass the token to the word
    Command command;
    if (current_token.value == NULL) current_token = get_next_token(lexer);

    if(current_token.type == TOK_BARE_WORD){
        Word word = get_word();
        command.program = word;
    }
    else{
        // this shouldn't return command but should trigger some kind of error;
        return command;
    }

    current_token = get_next_token(lexer);

    while(current_token.type == TOK_BARE_WORD || current_token.type == TOK_DOUBLE_QUOTED_WORD || current_token.type == TOK_SINGLE_QUOTED_WORD){
        // if the args array is empty, initialize it and place the value. If the array is not empty, increase the size and append the value
        
    }
    
}

Word get_word(){
    Word word = {current_token};
    // empty the current token
    empty_current_token(&current_token);
    return word;
}

void empty_current_token(Token *token){
    token->position = NULL;
    token->value = NULL;
    token ->type = NULL;
};


void reset_parser(Parser *parser){
    parser->mode = PS1;
    parser->root_node = NULL;
}