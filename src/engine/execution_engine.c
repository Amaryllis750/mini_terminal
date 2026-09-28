#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nodes.h"
#include "execution_engine.h"

ExecutionNode* generate_empty_node(Command cmd)
{
    ExecutionNode *node = malloc(sizeof(ExecutionNode));
    if (node == NULL) return NULL;
    
    node->cmd = cmd;
    node->stdin_type = REDIRECT_DEFAULT;
    node->stdout_type = REDIRECT_DEFAULT;
    node->stderr_type = REDIRECT_DEFAULT;
    node->stdin_target = NULL;
    node->stdout_target = NULL;
    node->stderr_target = NULL;
    node->stdin_file = NULL;
    node->stdout_file = NULL;
    node->stderr_file = NULL;
    node->next = NULL;
    
    return node;
}

ExecutionNode* get_exec_node(PipedCommand p)
{
    // Create the first node from r_command
    ExecutionNode *head = generate_empty_node(p.r_command.command);
    if (head == NULL) return NULL;
    
    // Apply redirections to the first node
    RedirectedArray *r_array = p.r_command.r_array;
    size_t r_array_size = p.r_command.r_array_size;
    
    int i;
    for (i = 0; i < r_array_size; i++)
    {
        RedirectedArray r_array_item = r_array[i];
        if (strcmp(r_array_item.redirection.value, ">") == 0)
        {
            head->stdout_type = REDIRECT_FILE;
            head->stdout_target = r_array_item.stream.value;
        }
        else if (strcmp(r_array_item.redirection.value, "2>") == 0)
        {
            head->stderr_type = REDIRECT_FILE;
            head->stderr_target = r_array_item.stream.value;
        }
        else if (strcmp(r_array_item.redirection.value, "<") == 0)
        {
            head->stdin_type = REDIRECT_FILE;
            head->stdin_target = r_array_item.stream.value;
        }
    }
    
    // Build the rest of the pipeline from p_array
    ExecutionNode *current = head;
    for (i = 0; i < p.p_array_size; i++)
    {
        PipedArray p_array_item = p.p_array[i];
        ExecutionNode *next = generate_empty_node(p_array_item.r_command.command);
        if (next == NULL) return head;  // partial pipeline is better than nothing
        
        current->next = next;
        
        // Apply redirections to this node
        RedirectedArray *inner_r_array = p_array_item.r_command.r_array;
        size_t inner_r_array_size = p_array_item.r_command.r_array_size;
        
        int j;
        for (j = 0; j < inner_r_array_size; j++)
        {
            RedirectedArray r_array_item = inner_r_array[j];
            if (strcmp(r_array_item.redirection.value, ">") == 0)
            {
                next->stdout_type = REDIRECT_FILE;
                next->stdout_target = r_array_item.stream.value;
            }
            else if (strcmp(r_array_item.redirection.value, "2>") == 0)
            {
                next->stderr_type = REDIRECT_FILE;
                next->stderr_target = r_array_item.stream.value;
            }
            else if (strcmp(r_array_item.redirection.value, "<") == 0)
            {
                next->stdin_type = REDIRECT_FILE;
                next->stdin_target = r_array_item.stream.value;
            }
        }
        
        current = next;
    }
    
    return head;
}

ExecutionNode** execute_tree(SequencedCommand s)
{
    // Create an array of pipeline heads
    ExecutionNode **e_array = malloc(sizeof(ExecutionNode*) * (s.s_array_size + 1));
    if (e_array == NULL) return NULL;
    
    // First pipeline
    e_array[0] = get_exec_node(s.p_command);
    
    // Subsequent pipelines from s_array
    int i;
    for (i = 0; i < s.s_array_size; i++)
    {
        e_array[i + 1] = get_exec_node(s.s_array[i].p_command);
    }
    
    return e_array;
}