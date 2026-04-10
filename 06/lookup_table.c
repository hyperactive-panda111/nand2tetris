#include "lookup_table.h"

// Jump Lookup Table
Mapping JumpTable[] = {
	{"null", 0},
	{"JGT", 1},
	{"JEQ", 2},
	{"JGE", 3},
	{"JLT", 4},
	{"JNE", 5},
	{"JLE", 6},
	{"JMP", 7}
};

//Comp Lookup Table
Mapping CompTable[] = {
	{"0", 42},
	{"1", 63},
	{"-1", 58},
	{"D", 12},
	{"A", 48},
	{"M", 48},
	{"!D", 13},
	{"!A", 49},
	{"!M", 49},
	{"-D", 15},
	{"-A", 51},
	{"-M", 51},
	{"D+1", 31},
	{"A+1", 55},
	{"M+1", 55},
	{"D-1", 14},
	{"A-1", 50},
	{"M-1", 50},
	{"D+A", 2},
	{"D+M", 2},
	{"D-A", 19},
	{"D-M", 19},
	{"A-D", 7},
	{"M-D", 7},
	{"D&A", 0},
	{"D&M", 0},
	{"D|A", 21},
	{"D|M", 21},
};

//Dest Lookup Table
Mapping DestTable[] = {
	{"null", 0},
	{"M", 1},
	{"D", 2},
	{"DM", 3},
	{"A", 4},
	{"AM", 5},
	{"AD", 6},
	{"ADM", 7},
};

 int JUMP_TABLE_SIZE = sizeof(JumpTable) / sizeof(JumpTable[0]);
 int DEST_TABLE_SIZE = sizeof(DestTable) / sizeof(DestTable[0]);
 int COMP_TABLE_SIZE = sizeof(CompTable) / sizeof(CompTable[0]);

//search functionality
uint16_t search_lookup_table(Mapping* array, int size, char* string) {
	int i;

	for (i = 0; i < size; i++) {
		if (strcmp(array[i].mnemonic, string) == 0)
			return array[i].value;
	}

	return 0xFFFF;
}