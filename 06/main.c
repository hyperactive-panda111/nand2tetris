#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "symbol_table.h"

int main(int argc, char* argv[]) {
	

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <input_file.asm>\n", argv[0]);
		return (1);
	}

	char out_name[256];
	strcpy(out_name, argv[1]);
	char* dot = strrchr(out_name, '.');
	if (dot) strcpy(dot, ".hack");

	FILE* file = fopen(argv[1], "r");
	FILE* out_file = fopen(out_name, "w");

	
	if (!file) {
		perror("Error opening file\n");
		return (1);
	}

	if (!out_file) {
		perror("Error creating output file");
		return (1);
	}


	create_symbol_table();

	//basic parser pass
	parserV1(file);

	//reset file pointer
	rewind(file);

	//2nd pass
	parserV2(file, out_file);

	free_symbol_table();

	fclose(file);
	fclose(out_file);
	
	return (0);
}