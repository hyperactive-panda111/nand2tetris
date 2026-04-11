#include "getCommandType.h"
#include <string.h>

CommandType getCommandType(char* line) {
	if (line == NULL || *line == '\0') return INVALID;
	if (line[0] == '@') return line[1] != '\0' ? A_INSTRUCTION : INVALID;
	if (line[0] == '(') {
		size_t len = strlen(line);
		return (len >= 3 && line[len - 1] == ')') ? L_INSTRUCTION : INVALID;
	}
	// C_INSTRUCTION
	return C_INSTRUCTION;
}