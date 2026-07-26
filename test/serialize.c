#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "nodes.h"
#include "lexer.h"
#include "serialize.h"

void serialize_parray_item(PipedArray pa, FILE *file);
void serialize_sarray_item(SequencedArray sa, FILE *file);

typedef void (*ArrayItemSerializer)(const void *item, FILE *file);

static void serialize_array(FILE *file, const char *label, size_t count, const void *items, size_t item_size, ArrayItemSerializer serialize_item)
{
    if (count == 0)
        return;

    fprintf(file, ",\"%s\": [", label);
    for (size_t i = 0; i < count; ++i)
    {
        fprintf(file, "{");
        serialize_item((const char *)items + i * item_size, file);
        fprintf(file, "}");
        if (i + 1 < count)
            fprintf(file, ",");
    }
    fprintf(file, "]");
}

static void serialize_token_array_item(const void *item, FILE *file)
{
    serialize_token(*(const Token *)item, file);
}

static void serialize_redirect_array_item(const void *item, FILE *file)
{
    const RedirectedArray *redirected_item = (const RedirectedArray *)item;
    fprintf(file, "\"token\": {\"value\": \"%s\", \"position\": %d},", redirected_item->redirection.value, redirected_item->redirection.position);
    serialize_token(redirected_item->stream, file);
}

static void serialize_piped_array_item(const void *item, FILE *file)
{
    serialize_parray_item(*(const PipedArray *)item, file);
}

static void serialize_sequenced_array_item(const void *item, FILE *file)
{
    serialize_sarray_item(*(const SequencedArray *)item, file);
}

void serialize_scommand(SequencedCommand s_command, FILE *file)
{
    fprintf(file, "\"sequencedCommand\": {\n");
    serialize_pcommand(s_command.p_command, file);
    serialize_array(file, "sequencedArray", s_command.s_array_size, s_command.s_array, sizeof(SequencedArray), serialize_sequenced_array_item);
    fprintf(file, "}");
}

void serialize_pcommand(PipedCommand p_command, FILE *file)
{
    fprintf(file, "\"pipedCommand\": {\n");
    serialize_rcommand(p_command.r_command, file);
    serialize_array(file, "pipedArray", p_command.p_array_size, p_command.p_array, sizeof(PipedArray), serialize_piped_array_item);
    fprintf(file, "}");
}

void serialize_token(Token t, FILE *file)
{
    fprintf(file, "\"token\": {");
    fprintf(file, "\"value\": \"%s\",", t.value);
    fprintf(file, "\"position\": %d", t.position);
    fprintf(file, "}");
}

void serialize_command(Command c, FILE *file)
{
    fprintf(file, "\"command\": {");
    fprintf(file, "\"program\": {");
    serialize_token(c.program, file);
    fprintf(file, "}");
    serialize_array(file, "args", c.arg_count, c.args, sizeof(Token), serialize_token_array_item);
    fprintf(file, "}");
}

void serialize_rcommand(RedirectedCommand r, FILE *file)
{
    fprintf(file, "\"redirectedCommand\": {");
    serialize_command(r.command, file);
    serialize_array(file, "redirectedArray", r.r_array_size, r.r_array, sizeof(RedirectedArray), serialize_redirect_array_item);
    fprintf(file, "}");
}

void serialize_parray_item(PipedArray pa, FILE *file)
{
    fprintf(file, "\"token\": {\"value\": \"%s\", \"position\": %d},", pa.pipe.value, pa.pipe.position);
    serialize_rcommand(pa.r_command, file);
}

void serialize_sarray_item(SequencedArray sa, FILE *file)
{
    fprintf(file, "\"token\": {\"value\": \"%s\", \"position\": %d},", sa.sequence.value, sa.sequence.position);
    serialize_pcommand(sa.p_command, file);
}
