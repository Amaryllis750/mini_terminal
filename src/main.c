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

    Parser parser = {PS1, NULL};
    Lexer lexer = {NULL, 0, 0, NULL};

    while (1)
    {
        // get the state of the parser
        ShellMode mode = parser.mode;
        printf("We are in %s mode\n", mode == PS1 ? "PS1" : "PS2");
        if (mode == PS1)
        {
            if (parser.root_node)
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
            parse_input(input, &parser, &lexer);
        }
        else
        {
            // get another string and use it to extend the input variable and then
            printf("%s>", "$");
            
            // get the extended input
            char extend[1024];
            fgets(input, 1024, stdin);
            char *extended_input = extend_input(input, extend);

            // let the lexer point to this extended input
            lexer.input = extended_input;   // this might be redundant though
            lexer.pos = lexer.start;

            parse_input(extended_input, &parser, &lexer);
        }
    }

    return 0;
}