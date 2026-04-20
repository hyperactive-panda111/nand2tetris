#include <stdio.h>
#include "getCommandType.h"
#include "tokenizer.h"


void parser(FILE* fp, FILE* out, int* counter, char* filename) {
	char raw_line[256];
	int line_number = 0;
	
	while (fgets(raw_line, sizeof(raw_line), fp) != NULL) {
		line_number++;
		TokenizedLine line = tokenize(raw_line);
		if (line.count == 0) continue;
		getCommand(line, out, counter, filename);
	}
}