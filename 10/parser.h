#ifndef PARSER_H
#define PARSER_H
#include "tokenizer.h"
#include "symbol_table.h"

typedef enum { SK_CONSTRUCTOR, SK_METHOD, SK_FUNCTION} SK_KIND;

typedef struct {
    TokenTable* table;
    int current;
    FILE* out;
    FILE* vm_out;
    char* classname;
    int classname_length;
    char* subroutine_name;
    int subroutine_name_length;
    SK_KIND subroutine_kind;
    ClassTable* class_table;
    SubTable* sub_table;
    int label_count;
} ParserState;

void parser(ParserState *p, char *buffer);

void compileClass(ParserState* p);
void compileClassVarDec(ParserState* p);
void compileExpression(ParserState* p);
int compileExpressionList(ParserState* p);
void compileSubroutineDec(ParserState* p);
void compileSubroutineBody(ParserState* p);
void compileSubroutineCall(ParserState* p, char* varname, int var_name_length);
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