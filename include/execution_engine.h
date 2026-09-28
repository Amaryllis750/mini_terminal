#ifndef EXECUTION_NODE_H
#define EXECUTION_NODE_H

#include <stdio.h>
#include "nodes.h"

typedef enum {
    REDIRECT_DEFAULT,   // use default (pipe to next node, or terminal if last)
    REDIRECT_FILE       // redirect to/from a file
} RedirectType;

typedef struct ExecutionNode {
    // Command and arguments
    Command cmd;
    
    // Redirection types and targets
    RedirectType stdin_type;
    RedirectType stdout_type;
    RedirectType stderr_type;
    
    char *stdin_target;     // filename if stdin_type == REDIRECT_FILE, NULL otherwise
    char *stdout_target;    // filename if stdout_type == REDIRECT_FILE, NULL otherwise
    char *stderr_target;    // filename if stderr_type == REDIRECT_FILE, NULL otherwise
    
    // Runtime file handles (populated during execution)
    FILE *stdin_file;       // NULL if using default
    FILE *stdout_file;      // NULL if using default
    FILE *stderr_file;      // NULL if using default
    
    // Pipeline structure (singly linked)
    struct ExecutionNode *next;
    
} ExecutionNode;

// Function declarations
ExecutionNode* generate_empty_node(Command cmd);
ExecutionNode* get_exec_node(PipedCommand p);
ExecutionNode** execute_tree(SequencedCommand s);

#endif