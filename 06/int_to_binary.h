#ifndef INT_TO_BINARY
#define INT_TO_BINARY

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "lookup_table.h"


void int_to_binary(char* line, int value, FILE* output_file);
void instruction_to_bits(char* str, char* dest, char* comp, char* jump, FILE *output_file);

#endif