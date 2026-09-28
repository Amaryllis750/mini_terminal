#ifndef EXECUTION_SERIALIZER_H
#define EXECUTION_SERIALIZER_H

#include <stddef.h>
#include "execution_engine.h"

// Convert a single command to JSON string (must be freed by caller)
char* command_to_json(Command *cmd);

// Convert redirect type enum to string
const char* redirect_type_to_string(RedirectType type);

// Escape special characters for JSON (must be freed by caller)
char* json_escape_string(const char *str);

// Convert a single execution node (and its pipeline) to JSON (must be freed by caller)
char* execution_node_to_json(ExecutionNode *node);

// Convert entire execution tree to JSON array string (must be freed by caller)
char* serialize_execution_tree(ExecutionNode **e_array, size_t array_size);

// Free JSON string allocated by serializer
void free_json_string(char *json_string);

#endif