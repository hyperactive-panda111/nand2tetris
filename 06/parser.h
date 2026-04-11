#ifndef PARSER
#define PARSER

#include <stdio.h>
#include <string.h>
#include "strip.h"
#include "lookup_table.h"
#include "int_to_binary.h"
#include "parse_c_instr.h"
#include "symbol_table.h"
#include "getCommandType.h"

#define MAX_VALUE 32767

void parserV1(FILE* fp);
void parserV2(FILE* fp, FILE* output_file);

#endif