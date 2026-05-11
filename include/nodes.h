#ifndef NODES_HEADER_H
#define NODES_HEADER_H

#include "lexer.h"

typedef struct{
    Token word;
}Word;

// Command = WORD WORD*
typedef struct{
    Word program;
    Word *args; // pointer to an array of Words
    size_t arg_count;
} Command;


typedef struct{
    Token redirection;
    Command command;
} RedirectedArray;

// RedirectedCommand = Command (REDIRECTION Command)*
typedef struct{
    Command command;
    RedirectedArray *r_array; // array of redirected commands
    size_t r_array_size;
} RedirectedCommand;

typedef struct{
    Token pipe;
    RedirectedCommand r_command;
} PipedArray;

// PipedCommand = RedirectedCommand ('|' RedirectedCommand)*
typedef struct{
    RedirectedCommand r_command;
    PipedArray *p_array; // array of piped commands
    size_t p_array_size;
} PipedCommand;

typedef struct{
    Token sequence;
    PipedCommand p_command;
} SequencedArray;

// SequencedCommand = PipedCommand (';' PipedCommand)*
typedef struct {
    PipedCommand p_command;
    SequencedArray *s_array; // array of sequenced commands
    size_t s_array_size;
} SequencedCommand;

#endif