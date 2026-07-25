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
    SequencedCommand s = init_scommand();
    s.p_command = piped_command(parser, lexer);
    if (SHOULD_ABORT(parser))
        return s;

    // parser logic here...
    while (peek(parser).type == TOK_SEQUENCE)
    {
        SequencedArray s_array;
        Token sequence = consume(parser, lexer);
        s_array.sequence = sequence;

        PipedCommand p = piped_command(parser, lexer);
        if (SHOULD_ABORT(parser))
            return s;

        s_array.p_command = p;
        s.s_array_size++;
        SequencedArray *temp = s.s_array;
        s.s_array = realloc(s.s_array, (s.s_array_size) * sizeof(SequencedArray));
        if (s.s_array == NULL)
        {
            s.s_array = temp;
            parser->err_msg = "Memory Allocation failed";
            parser->error = PARSER_ERR_MEMORY;
            return s;
        }
        s.s_array[(s.s_array_size - 1)] = s_array;
    }
    if (peek(parser).type == INCOMPLETE_TOKEN)
    {
        handle_incomplete_token(parser);
        return s;
    }
    return s;
}

PipedCommand piped_command(Parser *parser, Lexer *lexer)
{
    PipedCommand p = init_pcommand();
    p.r_command = redirected_command(parser, lexer);

    if (SHOULD_ABORT(parser))
        return p;

    while (peek(parser).type == TOK_PIPE)
    {
        PipedArray p_array;
        Token pipe = consume(parser, lexer);
        if (SHOULD_ABORT(parser))
            return p;
        p_array.pipe = pipe;

        // get the redirected command
        RedirectedCommand r = redirected_command(parser, lexer);
        if (r.command.program.value == NULL)
            parser->mode = PS2; // you should enter a PS2 mode here;

        if (SHOULD_ABORT(parser))
            return p;

        p_array.r_command = r;
        p.p_array_size++;
        PipedArray *temp = p.p_array;
        p.p_array = realloc(p.p_array, (p.p_array_size) * sizeof(PipedArray));
        if (p.p_array == NULL)
        {
            p.p_array = temp;
            parser->error = PARSER_ERR_MEMORY;
            parser->err_msg = "Memory Allocation failed";
            return p;
        }
        p.p_array[(p.p_array_size - 1)] = p_array;
    }

    if (peek(parser).type == INCOMPLETE_TOKEN)
    {
        handle_incomplete_token(parser);
        return p;
    }

    return p;
}

RedirectedCommand redirected_command(Parser *parser, Lexer *lexer)
{
    // pass the token to the command
    RedirectedCommand r_command = init_rcommand();
    r_command.command = get_command(parser, lexer);

    // check the parser to see if there is any error
    if (SHOULD_ABORT(parser))
        return r_command;

    // parse the redirected command
    while (peek(parser).type == TOK_REDIRECT_STDERR ||
           peek(parser).type == TOK_REDIRECT_STDIN ||
           peek(parser).type == TOK_REDIRECT_STDOUT ||
           peek(parser).type == TOK_APPEND)
    {
        Token r_op = consume(parser, lexer);

        if (peek(parser).type != TOK_BARE_WORD &&
            peek(parser).type != TOK_DOUBLE_QUOTED_WORD &&
            peek(parser).type != TOK_SINGLE_QUOTED_WORD)
        {
            parser->error = PARSER_ERR_INVALID_VALUE;
            parser->err_msg = "Unexpected Token; expected redirection output token";
            parser->err_token = &parser->current_token;

            return r_command;
        }

        Token stream = consume(parser, lexer);
        RedirectedArray r_array = {r_op, stream};
        r_command.r_array_size++;
        RedirectedArray *temp = r_command.r_array;
        r_command.r_array = realloc(r_command.r_array, (r_command.r_array_size) * sizeof(RedirectedArray));
        if (r_command.r_array == NULL)
        {
            // memory allocation failed
            r_command.r_array = temp;
            parser->error = PARSER_ERR_MEMORY;
            parser->err_msg = "Memory allocation failed";
            return r_command;
        }
        r_command.r_array[r_command.r_array_size - 1] = r_array;
    }

    if (parser->current_token.type == INCOMPLETE_TOKEN)
    {
        handle_incomplete_token(parser);
        return r_command;
    }
    return r_command;
}

Command get_command(Parser *parser, Lexer *lexer)
{
    Command command = init_command();

    while (peek(parser).type == TOK_BARE_WORD ||
           peek(parser).type == TOK_DOUBLE_QUOTED_WORD ||
           peek(parser).type == TOK_SINGLE_QUOTED_WORD)
    {
        // convert all quoted tokens to bare tokens, quotes won't be passed to programs
        Token token = strip_quotes(consume(parser, lexer));
        if (command.program.type == TOK_NONE)
        {
            command.program = token;
        }
        else
        {
            command.arg_count++;
            Token *temp = command.args;
            command.args = realloc(command.args, sizeof(Token) * command.arg_count);
            if (command.args == NULL)
            {
                command.args = temp;
                parser->error = PARSER_ERR_MEMORY;
                parser->err_msg = "Memory allocation failed";
                return command;
            }
            command.args[command.arg_count - 1] = token;
        }
    }

    if (peek(parser).type == TOK_EOF)
    {
        return command;
    }
    else if (peek(parser).type == INCOMPLETE_TOKEN)
    {
        handle_incomplete_token(parser);
        return command;
    }
    return command;
}

Token peek(Parser *parser)
{
    return parser->current_token;
}

Token consume(Parser *parser, Lexer *l)
{
    // How does this function handle incomplete tokens, and how do you come back to where you stopped
    // How does this function handle an empty string -> "\n\0"

    Token token = parser->current_token;
    Token new_token = get_next_token(l);
    while (new_token.type == TOK_NEWLINE)
    {
        // advance the token
        new_token = get_next_token(l);
    }
    parser->current_token = new_token;
    return token;
}

void reset_parser(Parser *parser)
{
    parser->mode = PS1;
    parser->root = NULL;
}

void init_parser(Parser *p, Lexer *l)
{
    Token t = get_next_token(l);
    while (t.type == TOK_NEWLINE)
    {
        t = get_next_token(l);
    }
    p->current_token = t;
}

void handle_incomplete_token(Parser *p)
{
    p->error = PARSER_ERR_MISSING_VALUE;
    p->err_msg = strdup("Incomplete Token");
    p->err_token = &p->current_token;
    p->mode = PS2;
}

/*
Current Predicament:
I need to know how to handle when incomplete token comes in, if you assign the parser current token to an incomplete token,
it skips all checks because there is no parent check for incomplete tokens, the parent check is peek and peek never looks for an
incomplete token
*/

Token strip_quotes(Token t)
{
    if (t.value == NULL)
        return t;

    size_t len = strlen(t.value);
    if (len < 2)
        return t;

    char first = t.value[0];
    char last = t.value[len - 1];

    if ((first == '\'' && last == '\'') ||
        (first == '"' && last == '"'))
    {
        char *stripped = malloc(len - 1);
        if (stripped == NULL)
            return t;

        memcpy(stripped, t.value + 1, len - 2);
        stripped[len - 2] = '\0';

        free(t.value);
        t.value = stripped;
    }
    t.type = TOK_BARE_WORD;
    return t;
}