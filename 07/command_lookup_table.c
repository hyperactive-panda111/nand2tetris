#include "command_lookup_table.h"
#define TABLE_SIZE 31

size_t commandtable_size = sizeof(CommandTable) / sizeof(CommandTable[0]);
CommandNode* table[TABLE_SIZE] = {NULL};

//hash function
unsigned long hash_function(char* str) {
	unsigned long hash = 5381;
	int c;

	while ((c = *str++)) {
		hash = ((hash << 5) + hash) + c;
	}
	return hash % TABLE_SIZE;
}

//addEntry function
void addEntry(char* key, const CommandEntry* value) {
	unsigned long bucket = hash_function(key);

	CommandNode* ptr = malloc(sizeof(CommandNode));

	if (!ptr) {
		fprintf(stderr, "Malloc allocation error\n");
		exit(EXIT_FAILURE);
	}

	ptr->key = key;
	ptr->value = value;
	ptr->next = table[bucket];
	table[bucket] = ptr;
}

//add VM commands to table
void add_commands(void) {
	size_t i;

	for (i = 0; i < commandtable_size; i++)
		addEntry(CommandTable[i].name, &CommandTable[i]);
}


//initialize hash table
void create_lookup_table(void) {
	add_commands();
}

//search functionality
const CommandEntry* in_table(char* name) {
	unsigned long bucket = hash_function(name);
	CommandNode* ptr = table[bucket];

	while (ptr != NULL) {
		if (strcmp(ptr->key, name) == 0)
			return ptr->value;
		ptr = ptr->next;
	}
	//improve error message details
	fprintf(stderr, "Invalid keyword\n");
	exit(EXIT_FAILURE);
}

void free_lookup_table(void) {
	CommandNode *ptr, *tmp;
	size_t i;

	for (i = 0; i < TABLE_SIZE; i++) {
		ptr = table[i];

		while (ptr != NULL) {
			tmp = ptr;
			ptr = ptr->next;
			free(tmp);

		}

		table[i] = NULL;
	}
}