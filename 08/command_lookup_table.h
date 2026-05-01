#ifndef COMMAND_LOOKUP_TABLE_H
#define COMMAND_LOOKUP_TABLE_H


#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum {
	//arithmetic and logical
	C_ADD, C_SUB, C_NEG,
	C_EQ, C_GT, C_LT,
	C_AND, C_OR, C_NOT,

	//memory access
	C_POP, C_PUSH,

	//control flow
	C_LABEL, C_GOTO, C_IF_GOTO,

	//function
	C_FUNCTION, C_CALL, C_RETURN,

	C_UNKNOWN
} CommandType;

typedef enum {
	CAT_ARITHMETIC,
	CAT_MEMORY,
	CAT_FLOW,
	CAT_FUNCTION,
} CommandCategory;

typedef struct 
{
	char* name;
	CommandType type;
	CommandCategory category;
	int expected_tokens;
} CommandEntry;

//struct for lookup table
typedef struct CommandNode {
	char* key;
	const CommandEntry* value;
	struct CommandNode* next;
} CommandNode;

static const CommandEntry CommandTable[] = {
	{"add", C_ADD, CAT_ARITHMETIC, 1},
	{"sub", C_SUB, CAT_ARITHMETIC, 1},
	{"neg", C_NEG, CAT_ARITHMETIC, 1},
	{"eq", C_EQ, CAT_ARITHMETIC, 1},
	{"gt", C_GT, CAT_ARITHMETIC, 1},
	{"lt", C_LT, CAT_ARITHMETIC, 1},
	{"and", C_AND, CAT_ARITHMETIC, 1},
	{"or", C_OR, CAT_ARITHMETIC, 1},
	{"not", C_NOT, CAT_ARITHMETIC, 1},
	{"push", C_PUSH, CAT_MEMORY, 3},
	{"pop", C_POP, CAT_MEMORY, 3},
	{"label", C_LABEL, CAT_FLOW, 2},
	{"goto", C_GOTO, CAT_FLOW, 2},
	{"if-goto", C_IF_GOTO, CAT_FLOW, 2},
	{"function", C_FUNCTION, CAT_FUNCTION, 3},
	{"call", C_CALL, CAT_FUNCTION, 3},
	{"return", C_RETURN, CAT_FUNCTION, 1},
	//{"unknown", C_UNKNOWN, CAT_ERROR, 1},
};

extern size_t commandtable_size;

unsigned long hash_function(char* str);;
const CommandEntry* in_table(char* name);
void free_lookup_table(void);
void create_lookup_table(void);

#endif