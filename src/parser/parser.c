#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "nodes.h"

void parse_input(char *input, Parser *parser, Lexer *lexer)
{
    while (lexer->pos < strlen(input))
    {
        Token token = get_next_token(lexer);
        if (token.type == INCOMPLETE_TOKEN)
        {
            parser->mode = PS2;
            return;
        }
    }
    parser->mode = PS1; // if the parser has left the loop, it has gathered all tokens
}

/**
 * Each of the recursive functions will return a tree or subtree.
 * For the case of the sequence command, the entire tree will be returned since it is the highest node
 * For the other nodes, they would each return subtrees
 */

SequencedCommand sequence_command(Parser *parser, Lexer *lexer)
{
    Token token = get_next_token(lexer);

    // pass the token into the piped command
}

PipedCommand piped_command(Parser *parser, Lexer *lexer)
{
    // pass the token to the redirected command
    RedirectedCommand r_command = redirected_command(parser, lexer);
}

RedirectedCommand redirected_command(Parser *parser, Lexer *lexer)
{
    // pass the token to the command
    Command command = get_command(parser, lexer);

    RedirectedCommand r_command;
    r_command.command = command;

    // check the parser to see if there is any error
    if (parser->error != PARSER_OK)
        return r_command;

    // parse the redirected command
    r_command.r_array_size = 0;
    r_command.r_array = NULL;
    while (peek(parser).type == TOK_REDIRECT_STDERR ||
           peek(parser).type == TOK_REDIRECT_STDIN ||
           peek(parser).type == TOK_REDIRECT_STDOUT)
    {
        Token r_op = consume(parser, lexer);

        if (peek(parser).type != TOK_BARE_WORD &&
            peek(parser).type != TOK_DOUBLE_QUOTED_WORD &&
            peek(parser).type != TOK_SINGLE_QUOTED_WORD)
        {
            parser->error = PARSER_ERR_INVALID_VALUE;
            parser->err_msg = "Unexpected Token";
            parser->err_token = &parser->current_token;
        }

        Word stream = get_word(consume(parser, lexer));
        RedirectedArray r_array = {r_op, stream};
        r_command.r_array_size++;
        r_command.r_array = realloc(r_command.r_array, (r_command.r_array_size) * sizeof(Word));
        r_command.r_array[r_command.r_array_size - 1] = r_array;


    }

    return r_command;
}

Command get_command(Parser *parser, Lexer *lexer)
{
    // pass the token to the word
    Command command = {0};

    if (peek(parser).type != TOK_BARE_WORD)
    {
        parser->error = PARSER_ERR_INVALID_VALUE;
        parser->err_msg = strdup("Unexpected token here");
        parser->err_token = &parser->current_token;
        return command;
    }

    Word program = get_word(consume(parser, lexer));
    command.program = program;
    command.arg_count = 0;
    command.args = NULL;

    while (peek(parser).type == TOK_BARE_WORD ||
           peek(parser).type == TOK_DOUBLE_QUOTED_WORD ||
           peek(parser).type == TOK_SINGLE_QUOTED_WORD)
    {
        Token t = consume(parser, lexer);

        command.arg_count++;
        command.args = realloc(command.args, (command.arg_count) * sizeof(Word));
        command.args[command.arg_count - 1] = get_word(t);
    }

    return command;
}

Word get_word(Token token)
{
    Word word = {token};
    return word;
}

Token peek(Parser *parser)
{
    return parser->current_token;
}

Token consume(Parser *parser, Lexer *l)
{
    Token token = parser->current_token;
    parser->current_token = get_next_token(l);
    return token;
}

void reset_parser(Parser *parser)
{
    parser->mode = PS1;
    parser->root = NULL;
}