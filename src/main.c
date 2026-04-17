#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parser.h"
#include "lexer.h"

void execute(){
    printf("Excecute");
}


int main()
{
    char input[1024];

    Parser parser = {PS1, NULL};
    Lexer lexer = {NULL, 0, NULL};
    
    while(1){
        // get the state of the parser 
        ShellMode mode = parser.mode;
        if (mode == PS1){
            if(parser.root_node){
                execute();
            }
            fgets(input, 1024, stdin);
            //reset the parser and the lexer
            reset_parser(&parser);
            reset_lexer(&lexer);

            // parse the new input of the parser and the lexer
            parse_input(input, &parser, &lexer);
        }
        else{
            // get another string and use it to extend the input variable and then 
            return 0;
        }
    }
    
    return 0;
}