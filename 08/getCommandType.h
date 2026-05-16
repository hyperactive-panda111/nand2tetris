#ifndef GETCOMMANDTYPE_H
#define GETCOMMANDTYPE_H

#include <stdlib.h>
#include <stdio.h>
#include "tokenizer.h"
#include "command_lookup_table.h"
#include "string_to_int.h"
#include "assembly_conversion.h"

void getCommand(TokenizedLine info, FILE* out, int* counter, int* n_args, char* function_name, char* filename);
#endif