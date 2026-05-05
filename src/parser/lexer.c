#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

void lexer_init(Lexer *lexer, char *input)
{
    lexer->input = strdup(input);
    lexer->pos = 0;
    lexer->start = 0;
}

void reset_lexer(Lexer *l){
    free(l->input);
    l->pos = 0;
}

Token get_next_token(Lexer *lexer)
{

    // skip whitespace
    while (lexer->input[lexer->pos] == ' ' || lexer->input[lexer->pos] == '\t')
        lexer->pos++;

    lexer->start = lexer->pos; // save the start position before consuming

    if (lexer->input[lexer->pos] == '\0')
        return (Token){TOK_EOF, "\0", lexer->pos};
    
    if(lexer->input[lexer->pos] == '\n'){
        Token new_line = {TOK_NEWLINE, "\n", lexer->pos};
        lexer->pos++;
        return new_line;
    }
        

    // tokenize quoted words
    if (lexer->input[lexer->pos] == '"' || lexer->input[lexer->pos] == '\'') {
        char quote = lexer->input[lexer->pos];

        do{
            lexer->pos++;
        }
        while (lexer->input[lexer->pos] != quote && lexer->input[lexer->pos] != '\0');

        // verify that the quote was closed with the same kind
        if (lexer->input[lexer->pos] != quote) {
            lexer->err = "Unexpected token here, missing an ending quote";
            return (Token){INCOMPLETE_TOKEN, NULL, lexer->pos, };
        }

        lexer->pos++; // skip the closing quote
        int length = lexer->pos - lexer->start;
        char *token_value = strndup(lexer->input + lexer->start, length); // remember to free all these

        Token t = {};
        t.type     = (quote == '"') ? TOK_DOUBLE_QUOTED_WORD : TOK_SINGLE_QUOTED_WORD;
        t.value    = token_value;
        t.position = lexer->start + 1;
        return t;
    }

    // consume until whitespace or end of input or tab
    while (lexer->input[lexer->pos] != ' '  &&
           lexer->input[lexer->pos] != '\t' &&
           lexer->input[lexer->pos] != '\0')
        lexer->pos++;

    int length = lexer->pos - lexer->start;
    char *token_value = strndup(lexer->input + lexer->start, length);   // remember to free all this

    Token t = {};
    t.position = lexer->start + 1;

    if (strcmp(token_value, "|") == 0) {
        t.type  = TOK_PIPE;
        t.value = "|";
    }
    else if (strcmp(token_value, ">") == 0) {
        t.type  = TOK_REDIRECT_STDOUT;
        t.value = ">";
    }
    else if (strcmp(token_value, "<") == 0) {
        t.type  = TOK_REDIRECT_STDIN;
        t.value = "<";
    }
    else if (strcmp(token_value, "2>") == 0) {
        t.type  = TOK_REDIRECT_STDERR;
        t.value = "2>";
    }
    else if (strcmp(token_value, ">>") == 0) {
        t.type  = TOK_APPEND;
        t.value = ">>";
    }
    else if (strcmp(token_value, ";") == 0) {
        t.type  = TOK_SEQUENCE;
        t.value = ";";
    }
    else {
        t.type  = TOK_BARE_WORD;
        t.value = token_value;
    }

    return t;
}