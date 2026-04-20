#include <stdio.h>
#include "command_lookup_table.h"
#include "assembly_conversion.h"

void write_arithmetic(FILE* out, CommandType type, int* counter) {
	switch(type) {
		case C_ADD: write_binary(out, "D+M"); break;
		case C_SUB: write_binary(out, "M-D"); break;
		case C_AND: write_binary(out, "D&M"); break;
		case C_OR: write_binary(out, "D|M"); break;
		case C_EQ: write_comparison(out, "JEQ", counter); break;
		case C_GT: write_comparison(out, "JGT", counter); break;
		case C_LT: write_comparison(out, "JLT", counter); break;
		case C_NOT: write_unary(out, "!M"); break;
		case C_NEG: write_unary(out, "-M"); break;
		default:
			fprintf(stderr, "Error: non-arithmetic command passed to write arithmetic\n");
			exit(EXIT_FAILURE);
	}
}

void write_memory_access(FILE* out, CommandType type, char* segment, int* index, char* filename) {
	if (strcmp(segment, "constant") == 0)
		write_constant(out, type, index);
	else if (strcmp(segment, "static") == 0)
		write_static(out, type, index, filename);
	else if (strcmp(segment, "temp") == 0)
		write_fixed_base(out, type, index, "5");
	else if (strcmp(segment, "pointer") == 0)
		write_fixed_base(out, type, index, "3");
	else
		write_base_pointer(out, type, segment, index);
}


//write function for binary operators
void write_binary(FILE* out, char* code) {
	fprintf(out,
	 "@SP\n"
	 "AM=M-1\n"
	 "D=M\n"
	 "A=A-1\n"
	 "M=%s\n",
	 code);
}

//write function for unary operators
void write_unary(FILE* out, char* code) {
	fprintf(out,
		"@SP\n"
		"A=M-1\n"
		"M=%s\n",
		code);
}

//write function for logical operators - JEQ, JGT, JLT
void write_comparison(FILE* out, char* code, int* counter) {
	fprintf(out,
		"@SP\n"
		"AM=M-1\n"
		"D=M\n"
		"A=A-1\n"
		"D=M-D\n"
		"@LABEL_TRUE_%d\n"
		"D;%s\n"
		"@SP\n"
		"A=M-1\n"
		"M=0\n"
		"@LABEL_FALSE_%d\n"
		"0;JMP\n"
		"(LABEL_TRUE_%d)\n"
		"@SP\n"
		"A=M-1\n"
		"M=-1\n"
		"(LABEL_FALSE_%d)\n",
		*counter, code, *counter, *counter, *counter);
	(*counter)++;
}


//push/pop instructions
void write_constant(FILE* out, CommandType type, int* index) {
	if (type == C_POP) {
		fprintf(stderr, "Error: pop constant is not a valid VM command\n");
		exit(EXIT_FAILURE);
	}
	fprintf(out,
		"@%d\n"
		"D=A\n"
		"@SP\n"
		"A=M\n"
		"M=D\n"
		"@SP\n"
		"M=M+1\n",
		*index);
}

void write_static(FILE* out, CommandType type, int* index, char* filename) {
	if (type == C_PUSH) {
		fprintf(out,
			"@%s.%d\n"
			"D=M\n"
			"@SP\n"
			"A=M\n"
			"M=D\n"
			"@SP\n"
			"M=M+1\n",
			filename, *index);
	} else if (type == C_POP) {
		fprintf(out, 
			"@SP\n"
			"AM=M-1\n"
			"D=M\n"
			"@%s.%d\n"
			"M=D\n",
			filename, *index);
	}
}

void write_base_pointer(FILE* out, CommandType type, char* segment, int* index) {
	char* base;
	if (strcmp(segment, "local") == 0) base = "LCL";
	else if (strcmp(segment, "argument") == 0) base = "ARG";
	else if (strcmp(segment, "this") == 0) base = "THIS";
	else base = "THAT";

	if (type == C_PUSH) {
		fprintf(out,
			"@%d\n"
			"D=A\n"
			"@%s\n"
			"A=D+M\n"
			"D=M\n"
			"@SP\n"
			"A=M\n"
			"M=D\n"
			"@SP\n"
			"M=M+1\n",
			*index, base);
	} else if (type == C_POP) {
		fprintf(out,
			"@%d\n"
			"D=A\n"
			"@%s\n"
			"D=D+M\n"
			"@R13\n"
			"M=D\n"
			"@SP\n"
			"AM=M-1\n"
			"D=M\n"
			"@R13\n"
			"A=M\n"
			"M=D\n",
			*index, base);
	}
}

void write_fixed_base(FILE* out, CommandType type, int* index, char* base) {
	if (type == C_PUSH) {
		fprintf(out,
			"@%d\n"
			"D=A\n"
			"@%s\n"
			"A=D+A\n"
			"D=M\n"
			"@SP\n"
			"A=M\n"
			"M=D\n"
			"@SP\n"
			"M=M+1\n",
			*index, base);
	} else if (type == C_POP) {
		fprintf(out, 
			"@%d\n"
			"D=A\n"
			"@%s\n"
			"D=A+D\n"
			"@R13\n"
			"M=D\n"
			"@SP\n"
			"AM=M-1\n"
			"D=M\n"
			"@R13\n"
			"A=M\n"
			"M=D\n",
			*index, base);
	}
}