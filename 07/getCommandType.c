#include "getCommandType.h"

void getCommand(TokenizedLine info, FILE* out, int* counter, char* filename) {
	char* command_token = info.tokens[0];
	const CommandEntry* result = in_table(command_token);
	int cmd_code = result->type;

	if (info.count != result->expected_tokens) {
		fprintf(stderr, "Command structure misalignment\n");
		exit(EXIT_FAILURE);
	}

	switch(cmd_code) {
		//Arithmetic/Logical Operations
		case C_ADD:
		case C_SUB:
		case C_AND:
		case C_OR:
		case C_NEG:
		case C_NOT:
		case C_EQ:
		case C_LT:
		case C_GT:
			write_arithmetic(out, cmd_code, counter);
			break;
		//Push/Pop Stack Operations
		case C_POP:
		case C_PUSH: {
			char* segment = info.tokens[1];
			int offset = string_to_int(info.tokens[2]);
			write_memory_access(out, cmd_code, segment, &offset, filename);
			break;
		}
			
	}
}