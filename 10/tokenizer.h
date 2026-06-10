#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <string.h>


typedef enum {
    T_KEYWORD,
    T_SYMBOL,
    T_NUMBER,
    T_STRING,
    T_IDENTIFIER,
    //T_EOF //end of buffer indicator
} TokenType;

typedef enum {
    C_LETTER,
    C_DIGIT,
    C_STRING,
    C_SYMBOL,
    C_UNKNOWN
} CharType;

typedef struct {
    TokenType type;
    char* lexeme;
    int length;
} Token; 

typedef struct {
    Token* tokens;
    int capacity;
    int count;
} TokenTable;

void tokenizer(TokenTable* table_info, char* buffer);
void tokenize_string(char** anchor, TokenTable* table, CharType* char_table);
void tokenize_number(char** anchor, TokenTable* table, CharType* char_table);
void tokenize_identifier(char** anchor, TokenTable* table, CharType* char_table);
void tokenize_symbol(char** anchor, TokenTable* table);
void handle_comments(char** anchor);
void tokenize_eof(TokenTable* table, char* anchor);
int string_compare(const void* s1, const void* s2);
void print_tokenizer_xml(FILE *out, TokenTable* table);

#endif
