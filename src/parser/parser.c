#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "nodes.h"

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

SequencedCommand sequence_command(Parser *parser, Lexer *lexer){
    Token token = get_next_token(lexer);

    // pass the token into the piped command

}

PipedCommand piped_command(Parser *parser, Lexer *lexer){
    // pass the token to the redirected command
    RedirectedCommand r_command = redirected_command(parser, lexer);
    
}

RedirectedCommand redirected_command(Parser *parser, Lexer *lexer){
    // pass the token to the command
    Command command = get_command(parser, lexer);

    RedirectedCommand r_command;
    r_command.command = command;

    // check the parser to see if there is any error
    if(parser->error != PARSER_OK) return r_command;

    // parse the redirected command
}

Command get_command(Parser *parser, Lexer *lexer){
    // pass the token to the word
    Command command = {0};

    if(parser->current_token.type == TOK_NONE) parser->current_token = get_next_token(lexer);
    // handle when the current token is EOF
    if(parser->current_token.type == TOK_EOF) return command;

    if(parser->current_token.type != TOK_BARE_WORD) {
        parser->error = PARSER_ERR_INVALID_VALUE;
        parser->err_token = &parser->current_token;
        return command;
    }

    Word program = get_word(parser->current_token);
    command.program = program;
    command.arg_count = 0;
    
    parser->current_token = get_next_token(lexer);
    Word *args = (Word *) malloc(sizeof(Word));
    command.args = args;

    
    while(parser->current_token.type == TOK_BARE_WORD || parser->current_token.type == TOK_DOUBLE_QUOTED_WORD || parser->current_token.type == TOK_SINGLE_QUOTED_WORD){
        if(command.arg_count > 0){
            // there is already an element in the array
            // resize the array and insert the new element

            Word *temp = args;
            args = realloc(args, (command.arg_count + 1) * sizeof(Word));
            if(!args){
                args = temp;
                printf("Reallocation of memory failed");
                // throw some error here
                parser->error = PARSER_ERR_MEMORY;

                return command; // return the command
            }
            args[command.arg_count] = get_word(parser->current_token);
        }
        else{
            // the first element in the array
            args[command.arg_count] = get_word(parser->current_token);
            command.arg_count++;
        }

        parser->current_token = get_next_token(lexer);
    }
    if(command.arg_count == 0) free(args);
    return command;
}

Word get_word(Token token){
    Word word = {token};
    return word;
}


void reset_parser(Parser *parser){
    parser->mode = PS1;
    parser->root = NULL;
}