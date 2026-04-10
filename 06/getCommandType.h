#ifndef GET_COMMAND_TYPE
#define GET_COMMAND_TYPE

#include <ctype.h>
#include <stdlib.h>
#include <stddef.h>

typedef enum {
	A_INSTRUCTION,
	C_INSTRUCTION,
	L_INSTRUCTION,
	INVALID
} CommandType;

CommandType getCommandType(char* line);

#endif