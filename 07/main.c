#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "command_lookup_table.h"
#include "parser.h"

void verify_output_filename(char *out_name) {
	int i, len = strlen(out_name);
	if (*out_name >= 'a' && *out_name <= 'z') {
		*out_name = toupper((unsigned char) *out_name);
	}	
}

int main(int argc, char* argv[]) {
	int counter = 0;
	char* stem_start;
	size_t stem_len;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <input_file.vm>\n", argv[0]);
		return (EXIT_FAILURE);
	}
	
	char out_name[256];
	char base_name[256];
	
	if (snprintf(out_name, sizeof(out_name), "%s", argv[1]) >= (int)sizeof(out_name)) {
		fprintf(stderr, "Input path too long\n");
		exit(EXIT_FAILURE);
	}

	char* slash = strrchr(out_name, '/');
	char* dot = strrchr(out_name, '.');

	if (!dot || (slash && dot < slash)) {
		fprintf(stderr, "Error: input file must have .vm extension\n");
		exit(EXIT_FAILURE);
	}
	
	if (slash) {
		stem_start = slash + 1;
		stem_len = dot - stem_start;
		strncpy(base_name, stem_start, stem_len);
		*(base_name + stem_len) = '\0';
	} else {
		stem_start = out_name;
		stem_len = dot - stem_start;
		strncpy(base_name, stem_start, stem_len);
		*(base_name + stem_len) = '\0';
	}

	snprintf(dot, sizeof(out_name) - (size_t)(dot - out_name), ".asm");
	
	//ensure output filename adheres to standard
	verify_output_filename(base_name);
	

	FILE* file = fopen(argv[1], "r");
	

	
	if (!file) {
		perror("Error opening file");
		return (EXIT_FAILURE);
	}
	FILE* out_file = fopen(out_name, "w");
	if (!out_file) {
		perror("Error creating output file");
		fclose(file);
		return (EXIT_FAILURE);
	}
	
	create_lookup_table();
	parser(file, out_file, &counter, base_name);
	free_lookup_table();

	fclose(file);
	fclose(out_file);


	return (EXIT_SUCCESS);
}
