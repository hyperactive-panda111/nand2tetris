#include "string_to_int.h"

int string_to_int(const char* input) {
	int sign = 1;
	long accumulator = 0;
	const char* str_ptr = input;

	if (input == NULL) {
		fprintf(stderr, "Invalid Input:\n");
		exit(EXIT_FAILURE);
	}

	if (*input == '\0') {
		fprintf(stderr, "Malformed VM Command:\n");
		exit(EXIT_FAILURE);
	}
		
	//skip leading whitespace
	while (isspace((unsigned char) *input)) input++;

	if (*input == '-') {
		sign = -1;
		input++;
	}
	if (*input < '0' || *input > '9') {
		fprintf(stderr, "Invalid Input. The input %s contains an invalid character :\n", input);
		exit(EXIT_FAILURE);
	}

	while ((*input >= '0' && *input <= '9') && *input != '\0') {
				
		accumulator = accumulator * 10 + (*input - '0');
		if ((sign == 1 && accumulator > (long)HACK_INT_MAX) || (sign == -1 && accumulator > -(long)HACK_INT_MIN)) {
			fprintf(stderr, "input value: %s causes overflow.\n", str_ptr);
				exit(EXIT_FAILURE);
		}
		input++;
	}

	accumulator *= sign;
	return (int)accumulator;

}

// int main(void) {
// 	char test[] = "12345";
// 	char test_neg[] = "-123";
// 	int result = string_to_int(test);
// 	int neg_result = string_to_int(test_neg);

// 	printf("%d\n", result);
// 	printf("%d\n", neg_result);
// 	return (0);
// }

