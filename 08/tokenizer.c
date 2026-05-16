#include <ctype.h>
#include "strip.h"
#include "tokenizer.h"

TokenizedLine tokenize(char* line) {
	int reached_end;
	char *anchor, *scout;
	TokenizedLine result;
	result.count = 0;

	for (int i = 0; i < 3; i++)
		result.tokens[i] = NULL;

	if (line == NULL) return result;

	//sanitize the input buffer
	anchor = strip(line);

	//line is a comment
	if (*anchor == '\0') return result;
	
	while (*anchor != '\0' && result.count < 3) {
		scout = anchor;
		//scout search
		while (*scout != '\0' && !isspace((unsigned char) *scout)) scout++;
		
		reached_end = (*scout == '\0');
		result.tokens[result.count++] = anchor;
		
		if (reached_end) break;
		*scout = '\0';
		
		anchor = scout + 1;
		while (*anchor != '\0' && isspace((unsigned char) *anchor)) anchor++;	
	}

	return result;
}