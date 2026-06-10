#include "parser_utilites.h"
#include "parser.h"
#include "symbol_table.h"
#include "tokenizer.h"
#include <stdio.h>
#include <stdlib.h>

Token peek(ParserState *p) { return p->table->tokens[p->current]; }

void advance(ParserState *p) {
  if (p->current < p->table->count) {
    p->current = p->current + 1;
  } else
    return;
}

void expect(ParserState *p, TokenType expectedType,
            const char *expectedLexeme) {
  const char *const tag_names[] = {[T_KEYWORD] = "keyword",
                                   [T_SYMBOL] = "symbol",
                                   [T_NUMBER] = "integerConstant",
                                   [T_STRING] = "stringConstant",
                                   [T_IDENTIFIER] = "identifier"};
  Token current = p->table->tokens[p->current];

  if (current.type != expectedType) {
    fprintf(stderr, "Syntax Error: Expected type %s\n Received type: %s\n",
            tag_names[expectedType], tag_names[current.type]);
    exit(EXIT_FAILURE);
  }

  if (expectedLexeme &&
      strncmp(current.lexeme, expectedLexeme, current.length) != 0) {
    fprintf(stderr, "Syntax Error: Expected string value: %s", expectedLexeme);
    exit(EXIT_FAILURE);
  }

  print_xml(p);
  advance(p);
}

void print_xml(ParserState *p) {
  const char *const tag_names[] = {[T_KEYWORD] = "keyword",
                                   [T_SYMBOL] = "symbol",
                                   [T_NUMBER] = "integerConstant",
                                   [T_STRING] = "stringConstant",
                                   [T_IDENTIFIER] = "identifier"};

  Token t = p->table->tokens[p->current];

  // Special handling for XML entities is still required
  if (t.type == T_SYMBOL) {
    const char *escaped = NULL;
    switch (*t.lexeme) {
    case '<':
      escaped = "&lt;";
      break;
    case '>':
      escaped = "&gt;";
      break;
    case '&':
      escaped = "&amp;";
      break;
    case '"':
      escaped = "&quot;";
      break;
    }

    if (escaped) {
      fprintf(p->out, "<symbol> %s </symbol>\n", escaped);
      return;
    }
  }

  // Fast path for everything else
  fprintf(p->out, "<%s> %.*s </%s>\n", tag_names[t.type], t.length, t.lexeme,
          tag_names[t.type]);
}

void writeTag(ParserState *p, const char *tag) {
  fprintf(p->out, "%s\n", tag);
  return;
}

void emit_push(FILE *vm_out, T_KIND kind, int index) {
  char *segment;

  switch (kind) {
  case T_ARG:
    segment = "argument";
    break;
  case T_STATIC:
    segment = "static";
    break;
  case T_FIELD:
    segment = "this";
    break;
  case T_VAR:
    segment = "local";
    break;
  default: {
    fprintf(stderr, "Unknown type\n");
    exit(EXIT_FAILURE);
  }
  }

  fprintf(vm_out, "push %s %d\n", segment, index);
}

void emit_pop(FILE *vm_out, T_KIND kind, int index) {
  char *segment;

  switch (kind) {
  case T_ARG:
    segment = "argument";
    break;
  case T_STATIC:
    segment = "static";
    break;
  case T_FIELD:
    segment = "this";
    break;
  case T_VAR:
    segment = "local";
    break;
  default: {
    fprintf(stderr, "Unknown type\n");
    exit(EXIT_FAILURE);
  }
  }

  fprintf(vm_out, "pop %s %d\n", segment, index);
}
void print_operator(ParserState *p, char *op) {
  switch (*op) {
  case '+':
    fprintf(p->vm_out, "add\n");
    break;
  case '-':
    fprintf(p->vm_out, "sub\n");
    break;
  case '&':
    fprintf(p->vm_out, "and\n");
    break;
  case '|':
    fprintf(p->vm_out, "or\n");
    break;
  case '<':
    fprintf(p->vm_out, "lt\n");
    break;
  case '>':
    fprintf(p->vm_out, "gt\n");
    break;
  case '=':
    fprintf(p->vm_out, "eq\n");
    break;
  case '/':
    fprintf(p->vm_out, "call Math.divide 2\n");
    break;
  case '*':
    fprintf(p->vm_out, "call Math.multiply 2\n");
    break;
  default: {
    fprintf(stderr, "Unrecognised token\n");
    exit(EXIT_FAILURE);
  }
  }
}

int isType(Token t) {
  if (t.type == T_IDENTIFIER)
    return 1;
  if (t.type == T_KEYWORD &&
      (compare_lexeme(t, "int") || compare_lexeme(t, "char") ||
       compare_lexeme(t, "boolean")))
    return 1;

  return 0;
}

int is_keyword_static_field(Token t) {
  if (t.type == T_KEYWORD &&
      (compare_lexeme(t, "field") || compare_lexeme(t, "static")))
    return 1;
  return 0;
}

int is_subroutine_keyword(Token t) {
  if (t.type == T_KEYWORD &&
      (compare_lexeme(t, "constructor") || compare_lexeme(t, "function") ||
       compare_lexeme(t, "method")))
    return 1;
  return 0;
}

int is_keyword_var(Token t) {
  if (t.type == T_KEYWORD && (compare_lexeme(t, "var")))
    return 1;
  return 0;
}

int is_statement(Token t) {
  if (t.type == T_KEYWORD &&
      (compare_lexeme(t, "let") || compare_lexeme(t, "do") ||
       compare_lexeme(t, "if") || compare_lexeme(t, "while") ||
       compare_lexeme(t, "return")))
    return 1;
  return 0;
}

int compare_lexeme(Token t, const char *str) {
  if (strlen(str) != (size_t)t.length)
    return 0;
  return strncmp(t.lexeme, str, t.length) == 0;
}

int is_binary_op(Token t) {
  if (t.type != T_SYMBOL || t.lexeme == NULL)
    return 0;

  const char *binary_ops = {"+-=<>|/*&"};

  return strchr(binary_ops, *(t.lexeme)) != NULL;
}

int get_priority(Token t) {
  if (t.type != T_SYMBOL || t.lexeme == NULL) {
    fprintf(stderr, "expected Jack recognised operator\n");
    exit(EXIT_FAILURE);
  }

  switch (*(t.lexeme)) {
  case '*':
  case '/':
    return 2;
    break;
  case '+':
  case '-':
    return 1;
    break;
  case '=':
  case '>':
  case '<':
  case '&':
  case '|':
    return 0;
    break;
  default: {
    fprintf(stderr, "expected Jack recognised operator: \"%c\"\n", *(t.lexeme));
    exit(EXIT_FAILURE);
  }
  }
}

int is_unary_op(Token t) {
  if (t.type != T_SYMBOL || t.lexeme == NULL)
    return 0;
  return (*(t.lexeme) == '-' || *(t.lexeme) == '~');
}

int is_keyword_constant(Token t) {
  if (t.type == T_KEYWORD &&
      (compare_lexeme(t, "true") || compare_lexeme(t, "false") ||
       compare_lexeme(t, "null") || compare_lexeme(t, "this")))
    return 1;
  return 0;
}
