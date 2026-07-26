#include <stdlib.h>
#include "parser.h"

typedef enum COMMAND_TYPE {
    SEQUENCED_COMMAND_INPUT=1, 
    PIPED_COMMAND_INPUT, 
    REDIRECTED_COMMAND_INPUT, 
    COMMAND_INPUT
} COMMAND_TYPE;

typedef struct ParserState{
    ShellMode parser_mode;
    ParserError parser_state;
    char *parser_err_msg;
} ParserState;

/*
@brief Defines a test case that will be created and run during testing
*/
typedef struct TestCase {
    /*@brief The input that you want to pass to the terminal */
    char *test_input;
    /* @brief The type of command that you want to test */
    COMMAND_TYPE input_type;
    /*@brief The expected state of the parser */
    ParserState parser_state;
} TestCase;