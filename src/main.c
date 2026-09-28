#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parser.h"
#include "lexer.h"
#include "terminal_limit.h"

void execute(Parser *parser)
{
    printf("Excecute\n");
    free_tree(*(parser->root));
    parser->root = NULL;
}

char *extend_input(char *original, char *extension)
{
    if (strlen(original) + strlen(extension) < INPUT_LIMIT)
    {
        return strcat(original, extension);
    }
    return NULL;
}

int main()
{
    char input[INPUT_LIMIT];

    Parser parser = {PS1, NULL, create_empty_token(), PARSER_OK, NULL, NULL}; // set the error to OK
    Lexer lexer = {NULL, 0, 0, NULL};

    while (1)
    {
        // get the state of the parser
        ShellMode mode = parser.mode;
        if (mode == PS1)
        {
            if (parser.root != NULL)
            {
                execute(&parser);
            }
            else
            {
                printf("%s>", "my_terminal");
                fgets(input, 1024, stdin);

                // reset the parser and the lexer...
                reset_parser(&parser);
                reset_lexer(&lexer);

                // initialize the parser and the lexer...
                lexer_init(&lexer, input);
                init_parser(&parser, &lexer);

                SequencedCommand s_command = sequence_command(&parser, &lexer);
                parser.root = &s_command;
                if (parser.error == PARSER_ERR_UNKNOWN_FLAG ||
                    parser.error == PARSER_ERR_INVALID_VALUE ||
                    parser.error == PARSER_ERR_UNEXPECTED_ARG || 
                    parser.error == PARSER_ERR_MEMORY)
                {
                    exit(1); // there was an error
                }
            }
        }
        else
        {
            // get another string and use it to extend the input variable and then
            printf("%s>", "$");

            // get the extended input
            char extend[1024];
            fgets(extend, 1024, stdin);
            char *extended_input = extend_input(input, extend);

            if (extended_input == NULL)
            {
                printf("Buffer exceeded");
                continue;
            }

            // TODO you are also to free the memory here and start again
            free_tree(*(parser.root));
            reset_lexer(&lexer);
            reset_parser(&parser);

            lexer_init(&lexer, input);
            init_parser(&parser, &lexer);

            SequencedCommand s = sequence_command(&parser, &lexer);
            parser.root = &s;
        }
    }

    return 0;
}