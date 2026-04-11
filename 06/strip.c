#include <string.h>
#include <ctype.h>

//objective of strip function
//strip trailing and leading whitespace
//strip comments


char* strip(char* line) {
	if (line == NULL) return NULL;
	char* comment_part = strstr(line, "//");
	if (comment_part) *comment_part= '\0';
	
	while (*line && isspace((unsigned char) *line)) line++;
	if (*line == '\0') return line;
	
	char* end = line + strlen(line) - 1;
	
	while (end > line && isspace((unsigned char) *end)) end--;
	*(end + 1) = '\0';

	return line;	
}