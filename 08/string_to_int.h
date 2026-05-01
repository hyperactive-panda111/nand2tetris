#ifndef STRING_TO_INT_H
#define STRING_TO_INT_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

#define HACK_INT_MAX 32767
#define HACK_INT_MIN -32768

int string_to_int(const char* input);

#endif