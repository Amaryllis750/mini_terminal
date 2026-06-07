#include <stdlib.h>

#include "nodes.h"


void free_word(Word w){
    free(w.word.value);

    return;
}

void free_command(Command c){
    free_word(c.program);

    if(c.arg_count > 0){
        int i;
        for(i=0;i<c.arg_count;i++){
            Word arg = c.args[i];
            free_word(arg);
        }

        free(c.args);
    }

    return ;
}

void free_redirected_command(RedirectedCommand r){
    free_command(r.command);

    if(r.r_array_size > 0){
        int i;
        for(i = 0; i < r.r_array_size; i++){
            RedirectedArray r_item = r.r_array[i];
            free(r_item.redirection.value);

            free_word(r_item.stream);
        }

        free(r.r_array);
    }

    return ;
}

void free_piped_command(PipedCommand p){
    free_redirected_command(p.r_command);

    if(p.p_array_size > 0){
        int i;
        for (i = 0; i < p.p_array_size; i++){
            PipedArray p_item = p.p_array[i];
            free(p_item.pipe.value);
            free_redirected_command(p_item.r_command);
        }

        free(p.p_array);
    }

    return ;
}

void free_tree(SequencedCommand s){
    // parse the tree and remove clear all dynamically allocated memory
    free_piped_command(s.p_command);

    if(s.s_array_size > 0){
        int i;
        for (i = 0; i < s.s_array_size; i++){
            SequencedArray s_item = s.s_array[i];
            free(s_item.sequence.value);
            free_piped_command(s_item.p_command);
        }

        free(s.s_array);
    }

    return ;
}