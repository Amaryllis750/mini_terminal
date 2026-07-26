#ifndef SERIALIZE_HEADER_H
#define SERIALIZE_HEADER_H

#include <stdio.h>
#include "nodes.h"

void serialize_scommand(SequencedCommand s, FILE *f);
void serialize_pcommand(PipedCommand s, FILE *f);
void serialize_rcommand(RedirectedCommand r, FILE *f);
void serialize_command(Command c, FILE *f);
void serialize_token(Token w, FILE *f);
void serialize_parray_item(PipedArray pa, FILE *f);
void serialize_sarray_item(SequencedArray sa, FILE *f);

#endif
