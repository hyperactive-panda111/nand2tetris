#ifndef LOOKUP_TABLE
#define LOOKUP_TABLE

#include <stdint.h>
#include <string.h>
#include <stddef.h>

typedef struct {
	char *mnemonic;
	uint16_t value;
} Mapping;


extern const  Mapping JumpTable[];
extern const  Mapping CompTable[];
extern const  Mapping DestTable[];

extern const size_t JUMP_TABLE_SIZE;
extern const size_t DEST_TABLE_SIZE;
extern const size_t COMP_TABLE_SIZE;


uint16_t search_lookup_table(const Mapping* array, size_t size, const char* string);
#endif