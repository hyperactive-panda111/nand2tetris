#include "parse_c_instr.h"

void parse_c_intruction(char *line, char **dest, char **comp, char **jump) {
	*dest = "null";
	*jump = "null";

	char *equal_sign, *semicolon;

	equal_sign = strchr(line, '=');
	semicolon = strchr(line, ';');

	if (equal_sign) {
		*equal_sign = '\0';
		*dest = line;
		*comp = equal_sign + 1;
	} else {
		*comp = line;
	}
	if (semicolon) {
		*semicolon = '\0';
		*jump = semicolon + 1;
	}
}