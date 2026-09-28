#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "execution_engine.h"
#include "engine_serializer.h"

#define INITIAL_BUFFER_SIZE 1024
#define BUFFER_GROWTH_FACTOR 1.5

typedef struct {
    char *data;
    size_t size;
    size_t capacity;
} StringBuffer;

StringBuffer* buffer_create(void)
{
    StringBuffer *buf = malloc(sizeof(StringBuffer));
    buf->capacity = INITIAL_BUFFER_SIZE;
    buf->size = 0;
    buf->data = malloc(buf->capacity);
    buf->data[0] = '\0';
    return buf;
}

void buffer_append(StringBuffer *buf, const char *str)
{
    size_t len = strlen(str);
    while (buf->size + len >= buf->capacity)
    {
        buf->capacity *= BUFFER_GROWTH_FACTOR;
        buf->data = realloc(buf->data, buf->capacity);
    }
    strcat(buf->data, str);
    buf->size += len;
}

void buffer_append_char(StringBuffer *buf, char c)
{
    if (buf->size + 1 >= buf->capacity)
    {
        buf->capacity *= BUFFER_GROWTH_FACTOR;
        buf->data = realloc(buf->data, buf->capacity);
    }
    buf->data[buf->size++] = c;
    buf->data[buf->size] = '\0';
}

char* buffer_get(StringBuffer *buf)
{
    return buf->data;
}

void buffer_free(StringBuffer *buf)
{
    free(buf->data);
    free(buf);
}

// Escape special characters in JSON strings
char* json_escape_string(const char *str)
{
    if (str == NULL) return NULL;
    
    StringBuffer *buf = buffer_create();
    
    for (int i = 0; str[i] != '\0'; i++)
    {
        switch (str[i])
        {
            case '"':
                buffer_append(buf, "\\\"");
                break;
            case '\\':
                buffer_append(buf, "\\\\");
                break;
            case '\b':
                buffer_append(buf, "\\b");
                break;
            case '\f':
                buffer_append(buf, "\\f");
                break;
            case '\n':
                buffer_append(buf, "\\n");
                break;
            case '\r':
                buffer_append(buf, "\\r");
                break;
            case '\t':
                buffer_append(buf, "\\t");
                break;
            default:
                buffer_append_char(buf, str[i]);
        }
    }
    
    char *result = malloc(strlen(buf->data) + 1);
    strcpy(result, buf->data);
    buffer_free(buf);
    return result;
}

const char* redirect_type_to_string(RedirectType type)
{
    switch (type)
    {
        case REDIRECT_DEFAULT:
            return "DEFAULT";
        case REDIRECT_FILE:
            return "FILE";
        default:
            return "UNKNOWN";
    }
}

// Forward declaration
char* execution_node_to_json(ExecutionNode *node);

char* command_to_json(Command *cmd)
{
    StringBuffer *buf = buffer_create();
    
    buffer_append(buf, "{\"program\":\"");
    char *escaped_prog = json_escape_string(cmd->program.value);
    buffer_append(buf, escaped_prog);
    free(escaped_prog);
    buffer_append(buf, "\",\"args\":[");
    
    for (int i = 0; i < cmd->arg_count; i++)
    {
        if (i > 0) buffer_append(buf, ",");
        buffer_append(buf, "\"");
        char *escaped_arg = json_escape_string(cmd->args[i].value);
        buffer_append(buf, escaped_arg);
        free(escaped_arg);
        buffer_append(buf, "\"");
    }
    
    buffer_append(buf, "]}");
    
    char *result = malloc(strlen(buf->data) + 1);
    strcpy(result, buf->data);
    buffer_free(buf);
    return result;
}

char* redirect_to_json(const char *name, RedirectType type, const char *target)
{
    StringBuffer *buf = buffer_create();
    
    buffer_append(buf, "\"");
    buffer_append(buf, name);
    buffer_append(buf, "\":{\"type\":\"");
    buffer_append(buf, redirect_type_to_string(type));
    buffer_append(buf, "\"");
    
    if (target != NULL)
    {
        buffer_append(buf, ",\"target\":\"");
        char *escaped_target = json_escape_string(target);
        buffer_append(buf, escaped_target);
        free(escaped_target);
        buffer_append(buf, "\"");
    }
    
    buffer_append(buf, "}");
    
    char *result = malloc(strlen(buf->data) + 1);
    strcpy(result, buf->data);
    buffer_free(buf);
    return result;
}

char* execution_node_to_json(ExecutionNode *node)
{
    if (node == NULL) return NULL;
    
    StringBuffer *buf = buffer_create();
    buffer_append(buf, "{");
    
    // Add command
    char *cmd_json = command_to_json(&node->cmd);
    buffer_append(buf, "\"cmd\":");
    buffer_append(buf, cmd_json);
    free(cmd_json);
    
    // Add stdin
    buffer_append(buf, ",");
    char *stdin_json = redirect_to_json("stdin", node->stdin_type, node->stdin_target);
    buffer_append(buf, stdin_json);
    free(stdin_json);
    
    // Add stdout
    buffer_append(buf, ",");
    char *stdout_json = redirect_to_json("stdout", node->stdout_type, node->stdout_target);
    buffer_append(buf, stdout_json);
    free(stdout_json);
    
    // Add stderr
    buffer_append(buf, ",");
    char *stderr_json = redirect_to_json("stderr", node->stderr_type, node->stderr_target);
    buffer_append(buf, stderr_json);
    free(stderr_json);
    
    // Add next node recursively
    if (node->next != NULL)
    {
        buffer_append(buf, ",\"next\":");
        char *next_json = execution_node_to_json(node->next);
        buffer_append(buf, next_json);
        free(next_json);
    }
    
    buffer_append(buf, "}");
    
    char *result = malloc(strlen(buf->data) + 1);
    strcpy(result, buf->data);
    buffer_free(buf);
    return result;
}

char* serialize_execution_tree(ExecutionNode **e_array, size_t array_size)
{
    StringBuffer *buf = buffer_create();
    buffer_append(buf, "[");
    
    for (int i = 0; i < array_size; i++)
    {
        if (i > 0) buffer_append(buf, ",");
        
        if (e_array[i] != NULL)
        {
            char *pipeline_json = execution_node_to_json(e_array[i]);
            buffer_append(buf, pipeline_json);
            free(pipeline_json);
        }
    }
    
    buffer_append(buf, "]");
    
    char *result = malloc(strlen(buf->data) + 1);
    strcpy(result, buf->data);
    buffer_free(buf);
    return result;
}

void free_json_string(char *json_string)
{
    free(json_string);
}