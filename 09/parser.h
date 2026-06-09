#ifndef PARSER_H
#define PARSER_H
#include "tokenizer.h"

typedef struct {
    TokenTable* table;
    int current;
    FILE* out;
} ParserState;

void parser(ParserState *p, char *buffer);

void compileClass(ParserState* p);
void compileClassVarDec(ParserState* p);
void compileExpression(ParserState* p);
int compileExpressionList(ParserState* p);
void compileSubroutineDec(ParserState* p);
void compileSubroutineBody(ParserState* p);
void compileSubroutineCall(ParserState* p);
void compileParameterList(ParserState* p);
void compileVarDec(ParserState* p);
void compileStatements(ParserState* p);
void compileLet(ParserState* p);
void compileIf(ParserState* p);
void compileWhile(ParserState* p);
void compileDo(ParserState* p);
void compileReturn(ParserState* p);
void compileTerm(ParserState* p);

#endif