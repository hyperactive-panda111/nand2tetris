#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "parser_utilites.h"
#include "symbol_table.h"

void parser(ParserState *p, char *buffer) {

  p->current = 0;
  p->label_count = 0;
  tokenizer(p->table, buffer);

  compileClass(p);
  if (p->current != p->table->count) {
    fprintf(stderr,
            "Syntax Error: unexpected tokens after class declaration.\n");
    exit(EXIT_FAILURE);
  }
}

void compileClass(ParserState *p) {
  init_class_table(p->class_table);
  writeTag(p, "<class>");

  expect(p, T_KEYWORD, "class");

  p->classname = p->table->tokens[p->current].lexeme;
  p->classname_length = p->table->tokens[p->current].length;

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
  // temp variables for symbol table row entry
  char *variable_name = NULL;
  size_t var_length;
  char *type = NULL;
  size_t type_length;
  T_KIND kind;

  writeTag(p, "<classVarDec>");

  if (compare_lexeme(p->table->tokens[p->current], "static"))
    kind = T_STATIC;
  else if (compare_lexeme(p->table->tokens[p->current], "field"))
    kind = T_FIELD;

  expect(p, T_KEYWORD, NULL);

  if (isType(peek(p))) {
    type = p->table->tokens[p->current].lexeme;
    type_length = p->table->tokens[p->current].length;

    print_xml(p);
    advance(p);
  }

  variable_name = p->table->tokens[p->current].lexeme;
  var_length = p->table->tokens[p->current].length;
  expect(p, T_IDENTIFIER, NULL);
  add_to_class_table(p->class_table, variable_name, var_length, type,
                     type_length, kind);

  while (peek(p).type == T_SYMBOL && *(peek(p).lexeme) == ',') {
    expect(p, T_SYMBOL, NULL);

    variable_name = p->table->tokens[p->current].lexeme;
    var_length = p->table->tokens[p->current].length;
    expect(p, T_IDENTIFIER, NULL);
    add_to_class_table(p->class_table, variable_name, var_length, type,
                       type_length, kind);
  }

  expect(p, T_SYMBOL, ";");
  writeTag(p, "</classVarDec>");
}

void compileSubroutineDec(ParserState *p) {
  writeTag(p, "<subroutineDec>");

  init_sub_table(p->sub_table);

  Token kind_tok = p->table->tokens[p->current];
  if (compare_lexeme(kind_tok, "constructor"))
    p->subroutine_kind = SK_CONSTRUCTOR;
  else if (compare_lexeme(kind_tok, "method"))
    p->subroutine_kind = SK_METHOD;
  else
    p->subroutine_kind = SK_FUNCTION;

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

  p->subroutine_name = p->table->tokens[p->current].lexeme;
  p->subroutine_name_length = p->table->tokens[p->current].length;

  if (p->subroutine_kind == SK_METHOD)
    add_to_sub_table(p->sub_table, "this", 4, p->classname, p->classname_length,
                     T_ARG);
  expect(p, T_IDENTIFIER, NULL);
  expect(p, T_SYMBOL, "(");
  compileParameterList(p);
  expect(p, T_SYMBOL, ")");
  compileSubroutineBody(p);
  writeTag(p, "</subroutineDec>");
}

void compileParameterList(ParserState *p) {
  // temp variables for symbol table row entry
  char *variable_name = NULL;
  size_t var_length;
  char *type = NULL;
  size_t type_length;

  writeTag(p, "<parameterList>");
  if (peek(p).type == T_SYMBOL && *(peek(p).lexeme) == ')') {
    writeTag(p, "</parameterList>");
    return;
  }
  if (isType(peek(p))) {
    type = p->table->tokens[p->current].lexeme;
    type_length = p->table->tokens[p->current].length;

    print_xml(p);
    advance(p);
  }

  variable_name = p->table->tokens[p->current].lexeme;
  var_length = p->table->tokens[p->current].length;
  expect(p, T_IDENTIFIER, NULL);
  add_to_sub_table(p->sub_table, variable_name, var_length, type, type_length,
                   T_ARG);

  while (peek(p).type == T_SYMBOL && *(peek(p).lexeme) == ',') {
    expect(p, T_SYMBOL, NULL);
    if (isType(peek(p))) {
      type = p->table->tokens[p->current].lexeme;
      type_length = p->table->tokens[p->current].length;

      print_xml(p);
      advance(p);
    }
    variable_name = p->table->tokens[p->current].lexeme;
    var_length = p->table->tokens[p->current].length;
    expect(p, T_IDENTIFIER, NULL);
    add_to_sub_table(p->sub_table, variable_name, var_length, type, type_length,
                     T_ARG);
  }
  writeTag(p, "</parameterList>");
}

void compileSubroutineBody(ParserState *p) {
  writeTag(p, "<subroutineBody>");
  expect(p, T_SYMBOL, "{");
  while (is_keyword_var(peek(p))) {
    compileVarDec(p);
  }

  fprintf(p->vm_out, "function %.*s.%.*s %d\n", (int)p->classname_length,
          p->classname, (int)p->subroutine_name_length, p->subroutine_name,
          p->sub_table->local_count);

  if (p->subroutine_kind == SK_CONSTRUCTOR) {
    fprintf(p->vm_out, "push constant %d\n", p->class_table->field_count);
    fprintf(p->vm_out, "call Memory.alloc 1\n");
    fprintf(p->vm_out, "pop pointer 0\n");
  } else if (p->subroutine_kind == SK_METHOD) {
    fprintf(p->vm_out, "push argument 0\n");
    fprintf(p->vm_out, "pop pointer 0\n");
  }

  compileStatements(p);
  expect(p, T_SYMBOL, "}");
  writeTag(p, "</subroutineBody>");
}

void compileVarDec(ParserState *p) {
  writeTag(p, "<varDec>");

  // temp variables for symbol table row entry
  char *variable_name = NULL;
  size_t var_length;
  char *type = NULL;
  size_t type_length;

  expect(p, T_KEYWORD, NULL); // var
  // type
  if (isType(peek(p))) {
    type = p->table->tokens[p->current].lexeme;
    type_length = p->table->tokens[p->current].length;
    print_xml(p);
    advance(p);
  }

  variable_name = p->table->tokens[p->current].lexeme;
  var_length = p->table->tokens[p->current].length;
  expect(p, T_IDENTIFIER, NULL); // varName
  add_to_sub_table(p->sub_table, variable_name, var_length, type, type_length,
                   T_VAR);

  while (peek(p).type == T_SYMBOL && *(peek(p).lexeme) == ',') {
    expect(p, T_SYMBOL, NULL);

    variable_name = p->table->tokens[p->current].lexeme;
    var_length = p->table->tokens[p->current].length;
    expect(p, T_IDENTIFIER, NULL);
    add_to_sub_table(p->sub_table, variable_name, var_length, type, type_length,
                     T_VAR);
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
  char *varname = NULL;
  int varname_length;
  int is_array = 0;

  writeTag(p, "<letStatement>");
  expect(p, T_KEYWORD, NULL);
  varname = p->table->tokens[p->current].lexeme;
  varname_length = p->table->tokens[p->current].length;

  expect(p, T_IDENTIFIER, NULL);
  // array handler
  if (peek(p).type == T_SYMBOL && *(peek(p).lexeme) == '[') {
    is_array = 1;
    SymbolTableEntry *result =
        in_sub_table(p->sub_table, varname, varname_length);
    if (!result)
      result = in_class_table(p->class_table, varname, varname_length);
    if (!result) {
      fprintf(stderr, "Undeclared variable name: %.*s\n", varname_length,
              varname);
      exit(EXIT_FAILURE);
    }

    expect(p, T_SYMBOL, NULL);
    compileExpression(p);
    emit_push(p->vm_out, result->kind, result->index);
    expect(p, T_SYMBOL, "]");
    fprintf(p->vm_out, "add\n");
  }
  expect(p, T_SYMBOL, "=");
  compileExpression(p);

  if (is_array) {
    fprintf(p->vm_out, "pop temp 0\n");
    fprintf(p->vm_out, "pop pointer 1\n");
    fprintf(p->vm_out, "push temp 0\n");
    fprintf(p->vm_out, "pop that 0\n");
  } else {
    SymbolTableEntry *result =
        in_sub_table(p->sub_table, varname, varname_length);
    if (!result)
      result = in_class_table(p->class_table, varname, varname_length);
    if (!result) {
      fprintf(stderr, "Unknown variable name\n");
      exit(EXIT_FAILURE);
    }
    emit_pop(p->vm_out, result->kind, result->index);
  }
  expect(p, T_SYMBOL, ";");
  writeTag(p, "</letStatement>");
}

void compileIf(ParserState *p) {
  int endblock_label = p->label_count++;
  int else_label = p->label_count++;

  writeTag(p, "<ifStatement>");
  expect(p, T_KEYWORD, NULL);
  expect(p, T_SYMBOL, "(");
  compileExpression(p);
  expect(p, T_SYMBOL, ")");

  fprintf(p->vm_out, "not\n");
  fprintf(p->vm_out, "if-goto %.*s_%d\n",
          (int)p->classname_length, p->classname, else_label);

  expect(p, T_SYMBOL, "{");
  compileStatements(p);
  expect(p, T_SYMBOL, "}");

  fprintf(p->vm_out, "goto %.*s_%d\n", (int)p->classname_length,
          p->classname, endblock_label);
  fprintf(p->vm_out, "label %.*s_%d\n", (int)p->classname_length,
          p->classname, else_label);

  if (peek(p).type == T_KEYWORD && compare_lexeme(peek(p), "else")) {
    expect(p, T_KEYWORD, NULL);
    expect(p, T_SYMBOL, "{");
    compileStatements(p);
    expect(p, T_SYMBOL, "}");
  }

  fprintf(p->vm_out, "label %.*s_%d\n", (int)p->classname_length,
          p->classname, endblock_label);
  writeTag(p, "</ifStatement>");
}

void compileWhile(ParserState *p) {
  int while_label = p->label_count++;
  int end_label = p->label_count++;

  writeTag(p, "<whileStatement>");
  expect(p, T_KEYWORD, NULL);
  expect(p, T_SYMBOL, "(");

  fprintf(p->vm_out, "label %.*s_%d\n", (int)p->classname_length, p->classname,
          while_label);
  compileExpression(p);
  fprintf(p->vm_out, "not\n");
  fprintf(p->vm_out, "if-goto %.*s_%d\n", (int)p->classname_length,
          p->classname, end_label);

  expect(p, T_SYMBOL, ")");
  expect(p, T_SYMBOL, "{");
  compileStatements(p);
  expect(p, T_SYMBOL, "}");

  fprintf(p->vm_out, "goto %.*s_%d\n", (int)p->classname_length, p->classname,
          while_label);
  fprintf(p->vm_out, "label %.*s_%d\n", (int)p->classname_length, p->classname,
          end_label);

  writeTag(p, "</whileStatement>");
}

void compileDo(ParserState *p) {
  writeTag(p, "<doStatement>");
  expect(p, T_KEYWORD, NULL);

  char *varname = p->table->tokens[p->current].lexeme;
  int varname_length = p->table->tokens[p->current].length;

  expect(p, T_IDENTIFIER, NULL);
  compileSubroutineCall(p, varname, varname_length);

  fprintf(p->vm_out, "pop temp 0\n");

  expect(p, T_SYMBOL, ";");
  writeTag(p, "</doStatement>");
}

void compileSubroutineCall(ParserState *p, char *varname, int var_name_length) {
  // subroutineName'(' expressionList ')'
  int nArgs = 0;

  if (peek(p).type == T_SYMBOL && *(peek(p).lexeme) == '(') {
    expect(p, T_SYMBOL, NULL);

    fprintf(p->vm_out, "push pointer 0\n");
    nArgs = compileExpressionList(p) + 1;
    expect(p, T_SYMBOL, ")");

    fprintf(p->vm_out, "call %.*s.%.*s %d\n", (int)p->classname_length,
            p->classname, var_name_length, varname, nArgs);
  }
  //(className | varName)'.'subroutineName'(' expressionList ')'
  else if (peek(p).type == T_SYMBOL && *(peek(p).lexeme) == '.') {
    expect(p, T_SYMBOL, NULL);

    char *subroutine_name = p->table->tokens[p->current].lexeme;
    int subroutine_name_length = p->table->tokens[p->current].length;
    expect(p, T_IDENTIFIER, NULL);
    expect(p, T_SYMBOL, "(");

    // look up varname in symbol tables (subroutine -> class)
    SymbolTableEntry *entry =
        in_sub_table(p->sub_table, varname, var_name_length);
    if (!entry)
      entry = in_class_table(p->class_table, varname, var_name_length);

    if (entry) {
      emit_push(p->vm_out, entry->kind, entry->index);
      nArgs = compileExpressionList(p) + 1;
      fprintf(p->vm_out, "call %.*s.%.*s %d\n", (int)entry->type_length,
              entry->type, subroutine_name_length, subroutine_name, nArgs);
    } else {
      // varname is a classname
      nArgs = compileExpressionList(p);
      fprintf(p->vm_out, "call %.*s.%.*s %d\n", var_name_length, varname,
              subroutine_name_length, subroutine_name, nArgs);
    }

    expect(p, T_SYMBOL, ")");
  } else {
    fprintf(stderr, "Syntax Error: Expected '.' or '(' in subroutine call. \n");
    exit(EXIT_FAILURE);
  }
}

void compileReturn(ParserState *p) {
  writeTag(p, "<returnStatement>");
  expect(p, T_KEYWORD, NULL);
  if (peek(p).type == T_SYMBOL && *(peek(p).lexeme) == ';') {
    expect(p, T_SYMBOL, NULL);
    fprintf(p->vm_out, "push constant 0\n");
    fprintf(p->vm_out, "return\n");
  }
  // expression
  else {
    compileExpression(p);
    expect(p, T_SYMBOL, ";");
    fprintf(p->vm_out, "return\n");
  }

  writeTag(p, "</returnStatement>");
}

void compileTerm(ParserState *p) {
  Token t = p->table->tokens[p->current];

  writeTag(p, "<term>");

  switch (t.type) {
  // integerConstant
  case T_NUMBER: {
    int length = t.length;
    char *str = t.lexeme;
    expect(p, T_NUMBER, NULL);

    fprintf(p->vm_out, "push constant %.*s\n", length, str);
    break;
  }
  // stringConstant
  case T_STRING: {
    int length = (int)t.length;
    char *str = t.lexeme;
    expect(p, T_STRING, NULL);

    fprintf(p->vm_out, "push constant %d\n", length);
    fprintf(p->vm_out, "call String.new 1\n");

    for (int i = 0; i < length; i++) {
      fprintf(p->vm_out, "push constant %d\n", (int)str[i]);
      fprintf(p->vm_out, "call String.appendChar 2\n");
    }
    break;
  } // keywordConstant
  case T_KEYWORD: {
    if (is_keyword_constant(t)) {
      if (compare_lexeme(t, "null") || compare_lexeme(t, "false"))
        fprintf(p->vm_out, "push constant 0\n");
      else if (compare_lexeme(t, "true"))
        fprintf(p->vm_out, "push constant 1\n"
                           "neg\n");
      else if (compare_lexeme(t, "this"))
        fprintf(p->vm_out, "push pointer 0\n");
    }
    expect(p, T_KEYWORD, NULL);
    break;
  }
  case T_IDENTIFIER: {
    char *varname = t.lexeme;
    int varname_length = (int)t.length;
    // varname | 1st token of subroutineCall
    expect(p, T_IDENTIFIER, NULL);
    // varname'[' expression ']'
    // TODO: emit array access sequence
    if (peek(p).type == T_SYMBOL && *(peek(p).lexeme) == '[') {
      SymbolTableEntry *entry =
          in_sub_table(p->sub_table, varname, varname_length);
      if (!entry)
        entry = in_class_table(p->class_table, varname, varname_length);
      if (!entry) {
        fprintf(stderr, "Error: Undeclared variable: %.*s\n", varname_length,
                varname);
        exit(EXIT_FAILURE);
      }
      expect(p, T_SYMBOL, "[");
      compileExpression(p);
      emit_push(p->vm_out, entry->kind, entry->index);
      expect(p, T_SYMBOL, "]");
      fprintf(p->vm_out, "add\n");
      fprintf(p->vm_out, "pop pointer 1\n");
      fprintf(p->vm_out, "push that 0\n");
    } // subroutineName'(' expressionList ')'
    else if (peek(p).type == T_SYMBOL &&
             (*(peek(p).lexeme) == '(' || *(peek(p).lexeme) == '.'))
      compileSubroutineCall(p, varname, varname_length);
    else {
      SymbolTableEntry *entry =
          in_sub_table(p->sub_table, varname, varname_length);
      if (!entry)
        entry = in_class_table(p->class_table, varname, varname_length);
      if (!entry) {
        fprintf(stderr, "Error: undeclared variable '%.*s\n", varname_length,
                varname);
        exit(EXIT_FAILURE);
      }
      emit_push(p->vm_out, entry->kind, entry->index);
    }
    break;
  }
  //(unaryOp term)
  case T_SYMBOL: {
    if (is_unary_op(t)) {
      char *symbol = t.lexeme;
      expect(p, T_SYMBOL, NULL);
      compileTerm(p);
      if (*symbol == '-')
        fprintf(p->vm_out, "neg\n");
      else if (*symbol == '~')
        fprintf(p->vm_out, "not\n");
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
  char *op = NULL;
  writeTag(p, "<expression>");

  compileTerm(p);
  while (is_binary_op(peek(p))) {
    op = p->table->tokens[p->current].lexeme;
    expect(p, T_SYMBOL, NULL);
    compileTerm(p);
    print_operator(p, op);
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
