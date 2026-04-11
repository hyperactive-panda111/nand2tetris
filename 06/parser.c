#include "parser.h"

void parserV1(FILE* fp) {
	char raw_line[256];
	size_t rom_counter = 0;

	while (fgets(raw_line, sizeof(raw_line), fp) != NULL) {

		char* str = strip(raw_line);

		switch (getCommandType(str)) {
			case INVALID:
				continue;
			case A_INSTRUCTION:
			case C_INSTRUCTION:
				rom_counter++;
				break;
			case L_INSTRUCTION: {
				char *label_start = str + 1;
		
				char *end = strchr(label_start , ')');

				*end = '\0';
					
				addEntry(label_start, rom_counter);
				break;
			}
		}
	}
}

void parserV2(FILE* fp, FILE* output_file) {
	char raw_line[256];
	size_t ram_counter = 16;
	char *dest, *comp, *jump;
	
	while (fgets(raw_line, sizeof(raw_line), fp) != NULL) {
		char* str = strip(raw_line);

		switch (getCommandType(str)) {
			case INVALID:
			case L_INSTRUCTION:
				continue;
			case A_INSTRUCTION: {
				const char* token = str + 1;
				int all_digits = (*token != '\0');
				for (const char *p = token; *p; ++p) {
					if (*p < '0' || *p > '9') {
						all_digits = 0;
						break;
					}
				}
				if (all_digits) {
					int value = atoi(token);
					if (value > MAX_VALUE) {
					fprintf(stderr, "input value exceed maximum value of Hack architecture");
					exit(1);
				}
				int_to_binary(str + 1, value, output_file);
				break;
				}
				if (*token >= '0' && *token <= '9') {
					fprintf(stderr, "Invalid A-instruction literal: %s\n", token);
					exit(1);
				}
				//A-Instruction symbol type
				if (in_table(str + 1) == 0) {
					addEntry(str + 1, ram_counter);
					ram_counter++;
				}
				int_to_binary(str + 1, get_address(str + 1), output_file);
				break;
			}
			case C_INSTRUCTION:
				parse_c_intruction(str, &dest, &comp, &jump);
				instruction_to_bits(str, dest, comp, jump, output_file);
				break;
		}
	}
}