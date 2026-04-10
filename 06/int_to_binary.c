#include "int_to_binary.h"
static int first_write = 1;

//convert decimal integer to binary and output to text file
void int_to_binary(char* line, int value, FILE* output_file) {
	int i;
	uint16_t bitmask;
	
	for (i = 0; i < 16; i++) {
		bitmask = (1 << (15 - i));
		if (value & bitmask)
			line[i] = '1';
		else
			line[i] = '0';
	}

	line[16] = '\0';
	if (!first_write)
		fputc('\n', output_file);
	first_write = 0;
	fputs(line, output_file);
}

void instruction_to_bits(char* str, char* dest, char* comp, char* jump, FILE *output_file) {
	uint16_t instruction = 0xE000;
	uint16_t c_bits, d_bits, j_bits;
	char line[256];

	c_bits = search_lookup_table(CompTable, COMP_TABLE_SIZE, comp);
	d_bits = search_lookup_table(DestTable, DEST_TABLE_SIZE, dest);
	j_bits = search_lookup_table(JumpTable, JUMP_TABLE_SIZE, jump);
	if (strchr(comp, 'M'))
		instruction |= (1 << 12);
	
	instruction |= (c_bits << 6);
	instruction |= (d_bits << 3);
	instruction |= j_bits;

	int i;
	uint16_t bitmask;
	
	for (i = 0; i < 16; i++) {
		bitmask = (1 << (15 - i));
		if (instruction & bitmask)
			line[i] = '1';
		else
			line[i] = '0';
	}

	line[16] = '\0';
	if (!first_write)
		fputc('\n', output_file);
	first_write = 0;
	fputs(line, output_file);

}