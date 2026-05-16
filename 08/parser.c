#include <stdio.h>
#include "getCommandType.h"
#include "tokenizer.h"

void parser(FILE* fp, FILE* out, int* counter, char* filename) {
	char raw_line[256];
	int line_number = 0;
	char current_function_name[256] ={0};
	int function_nargs = 0;
	
	while (fgets(raw_line, sizeof(raw_line), fp) != NULL) {
		line_number++;

		//handles truncated buffers
		if (strchr(raw_line, '\n') == NULL && !feof(fp)) {
			fprintf(stderr, "Line %d exceeds parser buffer\n", line_number);
			int ch;
			while ((ch = fgetc(fp)) != '\n' && ch != EOF) {

			}
			exit(EXIT_FAILURE);
		}

		TokenizedLine line = tokenize(raw_line);
		if (line.count == 0) continue;
		getCommand(line, out, counter, &function_nargs, current_function_name, filename);
	}

	if (ferror(fp)) {
		perror("Error reading VM input");
		exit(EXIT_FAILURE);
	}

	// fprintf(out, 
	// 	"(END)\n"
	// 	"@END\n"
	// 	"0;JMP\n");
}