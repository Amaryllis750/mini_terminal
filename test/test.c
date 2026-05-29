#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "nodes.h"
#include "lexer.h"

void serialize_scommand(SequencedCommand s, FILE *f);
void serialize_pcommand(PipedCommand s, FILE *f);
void serialize_rcommand(RedirectedCommand r, FILE *f);
void serialize_command(Command c, FILE *f);
void serialize_word(Word w, FILE *f);

void serialize_sarray_item(SequencedArray sa, FILE *f);
void serialize_parray_item(PipedArray pa, FILE *f);

void serialize_scommand(SequencedCommand s_command, FILE *file)
{
    fprintf(file, "sequencedCommand: {\n");
    serialize_pcommand(s_command.p_command, file);
    if (s_command.s_array_size > 0)
    {
        int i;
        fprintf(file, ",");
        fprintf(file, "sequencedArray: [\n");
        for (i = 0; i < s_command.s_array_size; i++)
        {
            fprintf(file, "{");
            serialize_sarray_item(s_command.s_array[i], file);
            fprintf(file, "}");
            if (i != (s_command.s_array_size - 1))
                fprintf(file, ",");
        }
        fprintf(file, "]\n");
    }
    fprintf(file, "}");
}

void serialize_pcommand(PipedCommand p_command, FILE *file)
{
    fprintf(file, "pipedCommand: {\n");
    serialize_rcommand(p_command.r_command, file);
    if (p_command.p_array_size > 0)
    {
        int i;
        fprintf(file, ",");
        fprintf(file, "pipedArray: [\n");
        for (i = 0; i < p_command.p_array_size; i++)
        {
            fprintf(file, "{");
            serialize_parray_item(p_command.p_array[i], file);
            fprintf(file, "}");
            if (i != (p_command.p_array_size - 1))
                fprintf(file, ",");
        }
        fprintf(file, "]\n");
    }
    fprintf(file, "}");
}

void serialize_word(Word w, FILE *file)
{
    fprintf(file, "word: {");
    fprintf(file, "value: \"%s\",", w.word.value);
    fprintf(file, "position: %d", w.word.position);
    fprintf(file, "}");
}

void serialize_command(Command c, FILE *file)
{
    fprintf(file, "command: {");
    fprintf(file, "program: {");
    serialize_word(c.program, file);
    fprintf(file, "}");
    if (c.arg_count > 0)
    {
        int i;
        fprintf(file, ",");
        fprintf(file, "args: [");
        for (i = 0; i < c.arg_count; i++)
        {
            fprintf(file, "{");
            serialize_word(c.args[i], file);
            fprintf(file, "}");
            if (i != (c.arg_count - 1))
                fprintf(file, ",");
        }
        fprintf(file, "]");
    }
    fprintf(file, "}");
}

void serialize_rcommand(RedirectedCommand r, FILE *file)
{
    fprintf(file, "redirectedCommand: {");
    serialize_command(r.command, file);
    if (r.r_array_size > 0)
    {
        int i;
        fprintf(file, ",");
        fprintf(file, "redirectedArray: [");
        for (i = 0; i < r.r_array_size; i++)
        {
            fprintf(file, "{");
            fprintf(file, "token: {value: \"%s\", position: %d},", r.r_array[i].redirection.value, r.r_array[i].redirection.position);
            serialize_command(r.r_array[i].command, file);
            fprintf(file, "}");
            if (i != (r.r_array_size - 1))
                fprintf(file, ",");
        }
        fprintf(file, "]");
    }
    fprintf(file, "}");
}

void serialize_parray_item(PipedArray pa, FILE *file)
{
    fprintf(file, "token: {value: \"%s\", position: %d},", pa.pipe.value, pa.pipe.position);
    serialize_rcommand(pa.r_command, file);
}

void serialize_sarray_item(SequencedArray sa, FILE *file)
{
    fprintf(file, "token: {value: \"%s\", position: %d},", sa.sequence.value, sa.sequence.position);
    serialize_pcommand(sa.p_command, file);
}

int main()
{
    printf("Starting Test for Parser...\n");

    printf("Initializing Parser and Lexer...\n");
    Token empty_token = {TOK_NONE, NULL, -1};
    Parser parser = {PS1, NULL, empty_token, 0}; // set the error to OK
    Lexer lexer = {NULL, 0, 0, NULL};

    printf("Enter you input: ");
    char input[1024];
    fgets(input, 1024, stdin);

    lexer_init(&lexer, input);

    printf("Creating output stream...");
    FILE *out_stream = fopen("tree.json", "w"); // note: "w" not 'w'
    if (out_stream == NULL)
        return 1;
    fprintf(out_stream, "{\n");

    // find which node they wish to test
    printf("Which node on the AST do you wish to start from? \n");
    printf("1. SequencedCommand\n");
    printf("2. PipedCommand\n");
    printf("3. RedirectedCommand\n");
    printf("4. Command\n");

    char choice[2];
    fgets(choice, 2, stdin);
    int choice_int = atoi(choice);
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
        break;
    }
    case 2:
    {
        PipedCommand p = piped_command(&parser, &lexer);
        serialize_pcommand(p, out_stream);
        break;
    }
    case 3:
    {
        RedirectedCommand r = redirected_command(&parser, &lexer);
        serialize_rcommand(r, out_stream);
        break;
    }
    case 4:
    {
        Command c = get_command(&parser, &lexer);
        serialize_command(c, out_stream);
        break;
    }
    default:
        printf("Invalid Input\n");
        break;
    }

    fprintf(out_stream, "\n}");
    fclose(out_stream);

    return 0;
}