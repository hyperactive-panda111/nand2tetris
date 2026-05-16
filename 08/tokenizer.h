#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string.h>
#include <stdlib.h>

typedef struct {
	char* tokens[3];
	int count;
} TokenizedLine;

TokenizedLine tokenize(char* line);

#endif