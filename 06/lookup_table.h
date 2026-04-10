#ifndef LOOKUP_TABLE
#define LOOKUP_TABLE

#include <stdint.h>
#include <string.h>

typedef struct {
	char *mnemonic;
	uint16_t value;
} Mapping;


extern Mapping JumpTable[];
extern Mapping CompTable[];
extern Mapping DestTable[];

extern int JUMP_TABLE_SIZE;
extern int DEST_TABLE_SIZE;
extern int COMP_TABLE_SIZE;


uint16_t search_lookup_table(Mapping* array, int size, char* string);
#endif