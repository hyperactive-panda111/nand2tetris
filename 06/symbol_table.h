#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

#define TABLE_SIZE 1009

typedef struct Symbol {
	char* name;
	int address;
	struct Symbol* next;
} Symbol;

void create_symbol_table();
void free_symbol_table();
void addEntry(char *name, int address);
int in_table(char* entry);
int get_address(char* entry);


#endif