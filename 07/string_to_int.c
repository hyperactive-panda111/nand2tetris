#include "string_to_int.h"
//TODO: add error reporting - print message and exit
//add parameter:- pointer to int

int string_to_int(const char* input) {
	int sign = 1;
	int result = 0;
	long accumulator = 0;

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
		int place_value = *input - '0';
		
		accumulator = (result * 10) + place_value;
		if (accumulator > HACK_INT_MAX || accumulator < HACK_INT_MIN) {
			fprintf(stderr, "input value: %s causes overflow.\n", input);
				exit(EXIT_FAILURE);
		}
		result = (result * 10) + place_value;
		input++;
	}

	result *= sign;
	//actually return pointer to int = result
	return result;

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

