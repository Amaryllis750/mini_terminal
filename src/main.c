#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parser.h"
#include "lexer.h"
#include "terminal_limit.h"

void execute()
{
    printf("Excecute");
}

char *extend_input(char *original, char *extension)
{   
    if(strlen(original) + strlen(extension) < INPUT_LIMIT){
        return strcat(original, extension);
    }
    return NULL;
}

int main()
{
    char input[INPUT_LIMIT];

    Token empty_token = {TOK_NONE, NULL, -1};
    Parser parser = {PS1, NULL, empty_token, 0};  // set the error to OK
    Lexer lexer = {NULL, 0, 0, NULL};

    while (1)
    {
        // get the state of the parser
        ShellMode mode = parser.mode;
        if (mode == PS1)
        {
            if (parser.root)
            {
                execute();
            }
            printf("%s>", "my_terminal");
            fgets(input, 1024, stdin);
            // reset the parser and the lexer
            reset_parser(&parser);
            reset_lexer(&lexer);

            // parse the new input of the parser and the lexer
            lexer_init(&lexer, input);  // initialize the lexer

            init_parser(&parser, &lexer);
            SequencedCommand s_command = sequence_command(&parser, &lexer);
            if(parser.error != PARSER_OK){
                exit(1); // there was an error
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
            if(extended_input == NULL) {
                printf("Buffer exceeded");
                continue;
            }

            // let the lexer point to this extended input
            lexer.input = input;   // this might be redundant though
            //TODO you are also to free the memory here and start again
            sequence_command(&parser, &lexer);
        }
    }

    return 0;
}