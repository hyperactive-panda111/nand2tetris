#include "getCommandType.h"

CommandType getCommandType(char* line) {
	if (line == NULL || *line == '\0') return INVALID;
	if (line[0] == '@') return A_INSTRUCTION;
	if (line[0] == '(') return L_INSTRUCTION;
	// C_INSTRUCTION
	return C_INSTRUCTION;
}