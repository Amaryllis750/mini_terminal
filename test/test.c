#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "nodes.h"
#include "lexer.h"

void serialize_scommand(SequencedCommand s, FILE *f);
void serialize_pcommand(PipedCommand s, FILE *f);
void serialize_rcommand(RedirectedCommand r, FILE *f);
void serialize_command(Command c, FILE *f);
void serialize_token(Token w, FILE *f);

void serialize_sarray_item(SequencedArray sa, FILE *f);
void serialize_parray_item(PipedArray pa, FILE *f);

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

void confirm_parser_state(Parser *p){
    printf("Parser State: %s\n", p->error == PARSER_OK ? "PARSER_OK" : "PARSER_NOT_OK");
    printf("Parser Error: %s\n", p->err_msg);
    printf("Parser Mode: %s\n", p->mode == PS1 ? "PS1" : "PS2");
}

int main()
{
    puts("Starting Test for Parser...");

    puts("Initializing Parser and Lexer...");
    Parser parser = {PS1, NULL, create_empty_token(), 0}; // set the error to OK
    Lexer lexer = {NULL, 0, 0, NULL};

    printf("Enter you input: ");
    char input[1024];
    fgets(input, 1024, stdin);
    // char *input = "echo \"Name";
    lexer_init(&lexer, input);

    puts("Creating output stream...");
    FILE *out_stream = fopen("tree.json", "w"); // note: "w" not 'w'
    if (out_stream == NULL)
        return 1;
    fprintf(out_stream, "{\n");

    init_parser(&parser, &lexer);    // initialize the first token of the parser
    // find which node they wish to test
    puts("Which node on the AST do you wish to start from?");
    puts("1. SequencedCommand");
    puts("2. PipedCommand");
    puts("3. RedirectedCommand");
    puts("4. Command");

    char choice[2];
    fgets(choice, 2, stdin);
    int choice_int = atoi(choice);
    // int choice_int = 1;
    if (choice_int == 0)
    {
        printf("Invalid Choice\n");
        return 1;
    }

    switch (choice_int)
    {
    case 1:
    {
        SequencedCommand scommand = sequence_command(&parser, &lexer);
        serialize_scommand(scommand, out_stream);
        free_tree(scommand);
        break;
    }
    case 2:
    {
        PipedCommand p = piped_command(&parser, &lexer);
        serialize_pcommand(p, out_stream);
        free_piped_command(p);
        break;
    }
    case 3:
    {
        RedirectedCommand r = redirected_command(&parser, &lexer);
        serialize_rcommand(r, out_stream);
        free_redirected_command(r);
        break;
    }
    case 4:
    {
        Command c = get_command(&parser, &lexer);
        if (parser.error != PARSER_OK){
            printf("%s", parser.err_msg);
            printf("\n");
        }
        serialize_command(c, out_stream);
        free_command(c);
        break;
    }
    default:
        printf("Invalid Input\n");
        break;
    }

    fprintf(out_stream, "\n}");
    fclose(out_stream);
    confirm_parser_state(&parser);

    // reset your lexer
    reset_lexer(&lexer);

    return 0;
}