#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "parser_utilites.h"

void parser(ParserState *p, char *buffer) {
  p->current = 0;
  tokenizer(p->table, buffer);

  compileClass(p);
}

void compileClass(ParserState *p) {
  writeTag(p, "<class>");
  expect(p, T_KEYWORD, "class");
  expect(p, T_IDENTIFIER, NULL);
  expect(p, T_SYMBOL, "{");
  // body
  while (is_keyword_static_field(peek(p))) {
    compileClassVarDec(p);
  }

  while (is_subroutine_keyword(peek(p))) {
    compileSubroutineDec(p);
  }
  expect(p, T_SYMBOL, "}");
  writeTag(p, "</class>");
}

void compileClassVarDec(ParserState *p) {
  writeTag(p, "<classVarDec>");
  expect(p, T_KEYWORD, NULL);
  if (isType(peek(p))) {
    print_xml(p);
    advance(p);
  }
  expect(p, T_IDENTIFIER, NULL);

  while (peek(p).type == T_SYMBOL && *(peek(p).lexeme) == ',') {
    expect(p, T_SYMBOL, NULL);
    expect(p, T_IDENTIFIER, NULL);
  }

  expect(p, T_SYMBOL, ";");
  writeTag(p, "</classVarDec>");
}

void compileSubroutineDec(ParserState *p) {
  writeTag(p, "<subroutineDec>");
  expect(p, T_KEYWORD, NULL);
  if (peek(p).type == T_KEYWORD && compare_lexeme(peek(p), "void"))
    expect(p, T_KEYWORD, NULL);
  else if (isType(peek(p))) {
    print_xml(p);
    advance(p);
  } else {
    fprintf(stderr, "Unexpected type\n");
    exit(EXIT_FAILURE);
  }

  expect(p, T_IDENTIFIER, NULL);
  expect(p, T_SYMBOL, "(");
  compileParameterList(p);
  expect(p, T_SYMBOL, ")");
  compileSubroutineBody(p);
  writeTag(p, "</subroutineDec>");
}

void compileParameterList(ParserState *p) {
  writeTag(p, "<parameterList>");
  if (peek(p).type == T_SYMBOL && *(peek(p).lexeme) == ')') {
    writeTag(p, "</parameterList>");
    return;
  }
  if (isType(peek(p))) {
    print_xml(p);
    advance(p);
  }
  expect(p, T_IDENTIFIER, NULL);

  while (peek(p).type == T_SYMBOL && *(peek(p).lexeme) == ',') {
    expect(p, T_SYMBOL, NULL);
    if (isType(peek(p))) {
      print_xml(p);
      advance(p);
    }
    expect(p, T_IDENTIFIER, NULL);
  }
  writeTag(p, "</parameterList>");
}

void compileSubroutineBody(ParserState *p) {
  writeTag(p, "<subroutineBody>");
  expect(p, T_SYMBOL, "{");
  while (is_keyword_var(peek(p))) {
    compileVarDec(p);
  }

  compileStatements(p);
  expect(p, T_SYMBOL, "}");
  writeTag(p, "</subroutineBody>");
}

void compileVarDec(ParserState *p) {
  writeTag(p, "<varDec>");
  expect(p, T_KEYWORD, NULL); // var
  // type
  if (isType(peek(p))) {
    print_xml(p);
    advance(p);
  }
  expect(p, T_IDENTIFIER, NULL); // varName

  while (peek(p).type == T_SYMBOL && *(peek(p).lexeme) == ',') {
    expect(p, T_SYMBOL, NULL);
    expect(p, T_IDENTIFIER, NULL);
  }

  expect(p, T_SYMBOL, ";");
  writeTag(p, "</varDec>");
}

void compileStatements(ParserState *p) {
  writeTag(p, "<statements>");
  while (is_statement(peek(p))) {
    Token t = peek(p);

    if (compare_lexeme(t, "let"))
      compileLet(p);
    else if (compare_lexeme(t, "do"))
      compileDo(p);
    else if (compare_lexeme(t, "while"))
      compileWhile(p);
    else if (compare_lexeme(t, "if"))
      compileIf(p);
    else if (compare_lexeme(t, "return"))
      compileReturn(p);
    else {
      fprintf(stderr, "Expected statement token type\n");
      exit(EXIT_FAILURE);
    }
  }
  writeTag(p, "</statements>");
}

void compileLet(ParserState *p) {
  writeTag(p, "<letStatement>");
  expect(p, T_KEYWORD, NULL);
  expect(p, T_IDENTIFIER, NULL);
  if (peek(p).type == T_SYMBOL && *(peek(p).lexeme) == '[') {
    expect(p, T_SYMBOL, NULL);
    compileExpression(p);
    expect(p, T_SYMBOL, "]");
  }
  expect(p, T_SYMBOL, "=");
  compileExpression(p);
  expect(p, T_SYMBOL, ";");
  writeTag(p, "</letStatement>");
}

void compileIf(ParserState *p) {
  writeTag(p, "<ifStatement>");
  expect(p, T_KEYWORD, NULL);
  expect(p, T_SYMBOL, "(");
  compileExpression(p);
  expect(p, T_SYMBOL, ")");
  expect(p, T_SYMBOL, "{");
  compileStatements(p);
  expect(p, T_SYMBOL, "}");
  if (peek(p).type == T_KEYWORD && compare_lexeme(peek(p), "else")) {
    expect(p, T_KEYWORD, NULL);
    expect(p, T_SYMBOL, "{");
    compileStatements(p);
    expect(p, T_SYMBOL, "}");
  }
  writeTag(p, "</ifStatement>");
}

void compileWhile(ParserState *p) {
  writeTag(p, "<whileStatement>");
  expect(p, T_KEYWORD, NULL);
  expect(p, T_SYMBOL, "(");
  compileExpression(p);
  expect(p, T_SYMBOL, ")");
  expect(p, T_SYMBOL, "{");
  compileStatements(p);
  expect(p, T_SYMBOL, "}");
  writeTag(p, "</whileStatement>");
}

void compileDo(ParserState *p) {
  writeTag(p, "<doStatement>");
  expect(p, T_KEYWORD, NULL);
  expect(p, T_IDENTIFIER, NULL);
  compileSubroutineCall(p);
  expect(p, T_SYMBOL, ";");
  writeTag(p, "</doStatement>");
}

void compileSubroutineCall(ParserState *p) {
  // subroutineName'(' expressionList ')'
  if (peek(p).type == T_SYMBOL && *(peek(p).lexeme) == '(') {
    expect(p, T_SYMBOL, NULL);
    compileExpressionList(p);
    expect(p, T_SYMBOL, ")");
  }
  //(className | varName)'.'subroutineName'(' expressionList ')'
  else if (peek(p).type == T_SYMBOL && *(peek(p).lexeme) == '.') {
    expect(p, T_SYMBOL, NULL);
    expect(p, T_IDENTIFIER, NULL);
    expect(p, T_SYMBOL, "(");
    compileExpressionList(p);
    expect(p, T_SYMBOL, ")");
  } else {
    fprintf(stderr,
            "Syntax Error: Expected \'.\' or \'(\' in subroutine call. \n");
    exit(EXIT_FAILURE);
  }
}

void compileReturn(ParserState *p) {
  writeTag(p, "<returnStatement>");
  expect(p, T_KEYWORD, NULL);
  if (peek(p).type == T_SYMBOL && *(peek(p).lexeme) == ';')
    expect(p, T_SYMBOL, NULL);
  // expression
  else {
    compileExpression(p);
    expect(p, T_SYMBOL, ";");
  }

  writeTag(p, "</returnStatement>");
}

void compileTerm(ParserState *p) {
  Token t = p->table->tokens[p->current];
  writeTag(p, "<term>");

  switch (t.type) {
  // integerConstant
  case T_NUMBER:
    expect(p, T_NUMBER, NULL);
    break;
  // stringConstant
  case T_STRING:
    expect(p, T_STRING, NULL);
    break;
  // keywordConstant
  case T_KEYWORD:
    if (is_keyword_constant(t))
      expect(p, T_KEYWORD, NULL);
    break;
  case T_IDENTIFIER: {
    // varname | 1st token of subroutineCall
    expect(p, T_IDENTIFIER, NULL);
    // varname'[' expression ']'
    if (peek(p).type == T_SYMBOL && *(peek(p).lexeme) == '[') {
      expect(p, T_SYMBOL, "[");
      compileExpression(p);
      expect(p, T_SYMBOL, "]");
    } // subroutineName'(' expressionList ')'
    else if (peek(p).type == T_SYMBOL &&
             (*(peek(p).lexeme) == '(' || *(peek(p).lexeme) == '.'))
      compileSubroutineCall(p);
    break;
  }
  //(unaryOp term)
  case T_SYMBOL: {
    if (is_unary_op(t)) {
      expect(p, T_SYMBOL, NULL);
      compileTerm(p);
    }
    //'('expression')'
    else if (t.type == T_SYMBOL && *(t.lexeme) == '(') {
      expect(p, T_SYMBOL, "(");
      compileExpression(p);
      expect(p, T_SYMBOL, ")");
    }
    break;
  }
  }
  writeTag(p, "</term>");
}

void compileExpression(ParserState *p) {
  writeTag(p, "<expression>");

  compileTerm(p);
  while (is_binary_op(peek(p))) {
    expect(p, T_SYMBOL, NULL);
    compileTerm(p);
  }
  writeTag(p, "</expression>");
}

int compileExpressionList(ParserState *p) {
  writeTag(p, "<expressionList>");
  int num_expression = 0;

  if (peek(p).type == T_SYMBOL && *(peek(p).lexeme) == ')') {
    writeTag(p, "</expressionList>");
    return 0;
  }

  compileExpression(p);
  ++num_expression;

  while (peek(p).type == T_SYMBOL && *(peek(p).lexeme) == ',') {
    expect(p, T_SYMBOL, NULL);
    compileExpression(p);
    ++num_expression;
  }

  writeTag(p, "</expressionList>");
  return num_expression;
}
