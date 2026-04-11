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
	if (snprintf(out_name, sizeof(out_name), "%s", argv[1]) >= (int)sizeof(out_name)) {
		fprintf(stderr, "Input path too long\n");
		exit(1);
	}

	char* slash = strrchr(out_name, '/');
	char* dot = strrchr(out_name, '.');
	if (dot && (!slash || dot > slash)) {
		if (snprintf(dot, sizeof(out_name) - (size_t)(dot - out_name), ".hack") < 0) {
			fprintf(stderr, "Failed to build output filename\n");
			return (1);
		}
	} else {
		size_t len = strlen(out_name);
		if (len + strlen(".hack") >= sizeof(out_name)) {
			fprintf(stderr, "Output path too long\n");
			return (1);
		}
		strcat(out_name, ".hack");
	}

	if (strcmp(out_name, argv[1]) == 0) {
		fprintf(stderr, "Refusing to overwrite input file\n");
		return (1);
	}

	FILE* file = fopen(argv[1], "r");
	

	
	if (!file) {
		perror("Error opening file");
		return (1);
	}
	FILE* out_file = fopen(out_name, "w");
	if (!out_file) {
		perror("Error creating output file");
		fclose(file);
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