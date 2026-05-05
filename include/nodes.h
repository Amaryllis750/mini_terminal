#ifndef NODES_HEADER_H
#define NODES_HEADER_H

#include "lexer.h"

typedef struct{
    Token *word;
}Word;

// Command = WORD WORD*
typedef struct{
    Word *program;
    Word *args; // pointer to an array of Words
} Command;


typedef struct {
    Token *redir_op; // redirection operator, >, >> ...
    Command *command;
}Redirection;

// RedirectedCommand = RedirectedCommand REDIRECTION WORD | Command
typedef struct{
    Command *command;
    Redirection *redirs; // a linkedlist of Redirection
    int redir_num;  // the number of redirections e.g. ps aux | grep "bash" | echo has 2 redirects
} RedirectedCommand;

typedef struct{
    Token *pipe;
    RedirectedCommand *r_command;
} Piped;

// PipedCommand=PipedCommand '|' RedirectedCommand | RedirectedCommand
typedef struct {
    RedirectedCommand *r_commands; // linked list of redirected commands
    Piped *piped; 
    int piped_num;
} PipedCommand;


typedef struct {
    Token *sequence; // ;
    PipedCommand *p_command;
} Sequenced;

// SequencedCommand = SequencedCommand ';' PipedCommand | PipedCommand
typedef struct{
    PipedCommand *p_command;
    Sequenced *sequenced;    // linked list of sequenced commands
    int sequenced_num;
} SequencedCommand;

#endif