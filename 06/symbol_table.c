#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "symbol_table.h"

//The 23 predefined symbols of the Hack ISA
const Symbol PredefinedSymbols[] = {
	{"R0", 0},
	{"R1", 1},
	{"R2", 2},
	{"R3", 3},
	{"R4", 4},
	{"R5", 5},
	{"R6", 6},
	{"R7", 7},
	{"R8", 8},
	{"R9", 9},
	{"R10", 10},
	{"R11", 11},
	{"R12", 12},
	{"R13", 13},
	{"R14", 14},
	{"R15", 15},
	{"SP", 0},
	{"LCL", 1},
	{"ARG", 2},
	{"THIS", 3},
	{"THAT", 4},
	{"SCREEN", 16384},
	{"KBD", 24576},
};

int predef_table_size = sizeof(PredefinedSymbols) / sizeof(PredefinedSymbols[0]);


//Symbol Table Implementation - Hash Table
Symbol* SymbolTable[TABLE_SIZE] = {NULL};

//hash function
unsigned long hash_function(char* str) {
	unsigned long hash = 5381;
	int c;

	while ((c = *str++)) {
		hash = ((hash << 5) + hash) + c;
	}
	return hash % TABLE_SIZE;
}

//initial setup for symbol table
void add_predefined_symbols() {
	int i;

	for (i = 0; i < predef_table_size; i++)
		addEntry(PredefinedSymbols[i].name, PredefinedSymbols[i].address);
}

//symbol table constructor
void create_symbol_table() {
	add_predefined_symbols();
}

//function to check if proposed entry exists
int in_table(char* entry) {
	unsigned long key = hash_function(entry);

	Symbol* ptr = SymbolTable[key];
	while (ptr) {
		if (strcmp(ptr->name, entry) == 0)
			return 1;
		ptr = ptr->next;
	}
	return 0;
}

int get_address(char* entry) {
	unsigned long key = hash_function(entry);

	Symbol* ptr = SymbolTable[key];
	while (ptr) {
		if (strcmp(ptr->name, entry) == 0)
			return ptr->address;
		ptr = ptr->next;
	}
	return -1;
}

void addEntry(char *name, int address) {
	unsigned long key = hash_function(name);

	if (in_table(name) == 0) {
		Symbol* new_node = malloc(sizeof(Symbol));

		if (!new_node) {
			fprintf(stderr, "Malloc allocation error\n");
			exit(1);
		}

		new_node->name = strdup(name);
		if (!new_node->name) {
			free(new_node);
			fprintf(stderr, "Symbol name allocation error\n");
			exit(1);
		}
		new_node->address = address;
		new_node->next = SymbolTable[key];
		SymbolTable[key] = new_node;
	}
}

void free_symbol_table() {
	int i;
	Symbol *current, *temp;

	for (i = 0; i < TABLE_SIZE; i++) {
		 current = SymbolTable[i];

		while (current != NULL) {
			temp = current;
			current = current->next;

			free(temp->name);
			free(temp);
		}

		SymbolTable[i] = NULL;
	}
}