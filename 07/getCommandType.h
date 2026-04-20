#ifndef GETCOMMANDTYPE_H
#define GETCOMMANDTYPE_H

#include <stdlib.h>
#include <stdio.h>
#include "tokenizer.h"
#include "command_lookup_table.h"
#include "string_to_int.h"
#include "assembly_conversion.h"

//add int* line_number int variable for robust error reporting
void getCommand(TokenizedLine info, FILE* out, int* counter, char* filename);

#endif