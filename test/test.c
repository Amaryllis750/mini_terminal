#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "nodes.h"
#include "lexer.h"
#include "serialize.h"
#include "test.h"

TestCase test_cases[] = {
    {"echo daniel", COMMAND_INPUT, {PS1, PARSER_OK, NULL}},
    {"echo \"daniel\"", COMMAND_INPUT, {PS1, PARSER_OK, NULL}},
    {"echo \"daniel", COMMAND_INPUT, {PS2, PARSER_ERR_MISSING_VALUE, "Incomplete Token"}},
    {"echo daniel > daniel.txt", REDIRECTED_COMMAND_INPUT, {PS1, PARSER_OK, NULL}},
    {"echo daniel > ", REDIRECTED_COMMAND_INPUT, {PS1, PARSER_ERR_INVALID_VALUE, "Unexpected Token; expected redirection output token"}},
    {"echo daniel | grep d", PIPED_COMMAND_INPUT, {PS1, PARSER_OK, NULL}},
    {"echo daniel |", PIPED_COMMAND_INPUT, {PS2, PARSER_OK, NULL}}
};

void confirm_parser_state(Parser *p)
{
    printf("Parser State: %s\n", p->error == PARSER_OK ? "PARSER_OK" : "PARSER_NOT_OK");
    printf("Parser Error: %s\n", p->err_msg);
    printf("Parser Mode: %s\n", p->mode == PS1 ? "PS1" : "PS2");
}

static int expect_parser_state(Parser *parser, ParserState expected, const char *input)
{
    int passed = 1;

    if (parser->mode != expected.parser_mode)
    {
        printf("FAILED: [%s] expected mode %s but got %s\n",
               input,
               expected.parser_mode == PS1 ? "PS1" : "PS2",
               parser->mode == PS1 ? "PS1" : "PS2");
        passed = 0;
    }

    if (parser->error != expected.parser_state)
    {
        printf("FAILED: [%s] expected parser state %d but got %d\n",
               input,
               expected.parser_state,
               parser->error);
        passed = 0;
    }

    if (expected.parser_err_msg == NULL)
    {
        if (parser->err_msg != NULL && parser->err_msg[0] != '\0')
        {
            printf("FAILED: [%s] expected no error message but got '%s'\n", input, parser->err_msg);
            passed = 0;
        }
    }
    else if (parser->err_msg == NULL || strcmp(parser->err_msg, expected.parser_err_msg) != 0)
    {
        printf("FAILED: [%s] expected error message '%s' but got '%s'\n",
               input,
               expected.parser_err_msg,
               parser->err_msg == NULL ? "(null)" : parser->err_msg);
        passed = 0;
    }

    if (passed)
    {
        printf("PASS: %s\n", input);
    }

    return passed;
}

static int run_automated_tests(void)
{
    size_t test_count = sizeof(test_cases) / sizeof(test_cases[0]);
    int passed = 0;
    int failed = 0;

    puts("Running automated parser tests...");

    for (size_t i = 0; i < test_count; ++i)
    {
        Parser parser = {PS1, NULL, create_empty_token(), PARSER_OK, NULL, NULL};
        Lexer lexer = {NULL, 0, 0, NULL};

        lexer_init(&lexer, test_cases[i].test_input);
        init_parser(&parser, &lexer);

        switch (test_cases[i].input_type)
        {
        case SEQUENCED_COMMAND_INPUT:
        {
            SequencedCommand scommand = sequence_command(&parser, &lexer);
            free_tree(scommand);
            break;
        }
        case PIPED_COMMAND_INPUT:
        {
            PipedCommand p = piped_command(&parser, &lexer);
            free_piped_command(p);
            break;
        }
        case REDIRECTED_COMMAND_INPUT:
        {
            RedirectedCommand r = redirected_command(&parser, &lexer);
            free_redirected_command(r);
            break;
        }
        case COMMAND_INPUT:
        default:
        {
            Command c = get_command(&parser, &lexer);
            free_command(c);
            break;
        }
        }

        reset_lexer(&lexer);

        if (expect_parser_state(&parser, test_cases[i].parser_state, test_cases[i].test_input))
        {
            passed++;
        }
        else
        {
            failed++;
        }
    }

    printf("Automated tests finished: %d passed, %d failed\n", passed, failed);
    return failed == 0 ? 0 : 1;
}

static int run_custom_test(void)
{
    puts("Starting Test for Parser...");

    puts("Initializing Parser and Lexer...");
    Parser parser = {PS1, NULL, create_empty_token(), PARSER_OK, NULL, NULL};
    Lexer lexer = {NULL, 0, 0, NULL};

    printf("Enter you input: ");
    char input[1024];
    if (!fgets(input, sizeof(input), stdin))
    {
        return 1;
    }

    lexer_init(&lexer, input);

    puts("Creating output stream...");
    FILE *out_stream = fopen("tree.json", "w");
    if (out_stream == NULL)
        return 1;
    fprintf(out_stream, "{\n");

    init_parser(&parser, &lexer);

    puts("Which node on the AST do you wish to start from?");
    puts("1. SequencedCommand");
    puts("2. PipedCommand");
    puts("3. RedirectedCommand");
    puts("4. Command");

    char choice[8];
    if (!fgets(choice, sizeof(choice), stdin))
    {
        fclose(out_stream);
        return 1;
    }

    int choice_int = atoi(choice);
    if (choice_int == 0)
    {
        printf("Invalid Choice\n");
        fclose(out_stream);
        return 1;
    }

    switch (choice_int)
    {
    case SEQUENCED_COMMAND_INPUT:
    {
        SequencedCommand scommand = sequence_command(&parser, &lexer);
        serialize_scommand(scommand, out_stream);
        free_tree(scommand);
        break;
    }
    case PIPED_COMMAND_INPUT:
    {
        PipedCommand p = piped_command(&parser, &lexer);
        serialize_pcommand(p, out_stream);
        free_piped_command(p);
        break;
    }
    case REDIRECTED_COMMAND_INPUT:
    {
        RedirectedCommand r = redirected_command(&parser, &lexer);
        serialize_rcommand(r, out_stream);
        free_redirected_command(r);
        break;
    }
    case COMMAND_INPUT:
    {
        Command c = get_command(&parser, &lexer);
        if (parser.error != PARSER_OK)
        {
            printf("%s\n", parser.err_msg);
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

    reset_lexer(&lexer);
    return 0;
}

int main(void)
{
    puts("Choose test mode:");
    puts("1. Automated tests");
    puts("2. Custom test");

    char mode_choice[8];
    if (!fgets(mode_choice, sizeof(mode_choice), stdin))
    {
        return 1;
    }

    int mode_choice_int = atoi(mode_choice);
    switch (mode_choice_int)
    {
    case 1:
        return run_automated_tests();
    case 2:
        return run_custom_test();
    default:
        printf("Invalid choice\n");
        return 1;
    }
}