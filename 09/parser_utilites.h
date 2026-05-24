#ifndef PARSER_UTILITIES_H
#define PARSER_UTILITIES_H

#include "parser.h"
#include "tokenizer.h"

Token peek(ParserState *p);
void advance(ParserState *p);
void print_xml(ParserState *p);
void writeTag(ParserState *p, const char *tag);
void expect(ParserState *p, TokenType expectedType, const char *expectedLexeme);

int is_subroutine_keyword(Token t);
int is_keyword_static_field(Token t);
int is_keyword_var(Token t);
int compare_lexeme(Token t, const char *str);
int is_statement(Token t);
int get_priority(Token t);
int is_unary_op(Token t);
int isType(Token t);
int is_binary_op(Token t);
int is_keyword_constant(Token t);

#endif
