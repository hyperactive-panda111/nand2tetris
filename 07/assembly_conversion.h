#ifndef ASSEMBLY_CONVERSION_H
#define ASSEMBLY_CONVERSION_H


#include <string.h>
#include <stdlib.h>
#include "command_lookup_table.h"
#include "assembly_conversion.h"


//arithmetic/logic instructions
void write_arithmetic(FILE* out, CommandType type, int* counter);
void write_binary(FILE* out, char* code);
void write_unary(FILE* out, char* code);
void write_comparison(FILE* out, char* code, int* counter);

//push/pop instructions
void write_memory_access(FILE* out, CommandType type, char* segment, int* index, char* filename);
void write_constant(FILE* out,CommandType type, int* index);
void write_static(FILE* out, CommandType type, int* index, char* filename);
void write_fixed_base(FILE* out, CommandType type, int* index, char* base);
void write_base_pointer(FILE* out, CommandType type, char* segment, int* index);

#endif