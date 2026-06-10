#include "tokenizer.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>

void add_token(TokenTable *table, Token token);
int string_to_int(const char *str, size_t size);

const char *const keywords[] = {
    "boolean", "char",     "class", "constructor", "do",  "else",   "false",
    "field",   "function", "if",    "int",         "let", "method", "null",
    "return",  "static",   "this",  "true",        "var", "void",   "while"};
char tmp_tkn[1024]; // temporary storage for string comparison and integer
                    // conversion

void tokenizer(TokenTable *table_info, char *buffer) {
  int ch;
  char *anchor = buffer; // pointer into source buffer
  char *p = NULL;

  CharType char_table[256];
  const char *symbol = {"{}()[].,;+-*/&|<>=~"};

  // initialize character table
  for (ch = 0; ch < 256; ch++)
    char_table[ch] = C_UNKNOWN;
  for (ch = '0'; ch <= '9'; ch++)
    char_table[ch] = C_DIGIT;
  for (ch = 'a'; ch <= 'z'; ch++)
    char_table[ch] = C_LETTER;
  for (ch = 'A'; ch <= 'Z'; ch++)
    char_table[ch] = C_LETTER;
  while (*symbol) {
    char_table[(unsigned char)*symbol] = C_SYMBOL;
    ++symbol;
  }
  ch = '"';
  char_table[ch] = C_STRING;
  ch = '_';
  char_table[ch] = C_LETTER;

  while (*anchor != '\0') {
    // continuously skip whitespace and comments
    while (1) {
      p = anchor;
      // skip whitespace
      while (*anchor && isspace((unsigned char)*anchor))
        ++anchor;

      // handle comments
      handle_comments(&anchor);

      // if pointer changed position, restart process
      if (p == anchor)
        break;
    }

    if (*anchor == '\0')
      break;

    // lexical classifications
    switch (char_table[(unsigned char)*anchor]) {
    case C_DIGIT:
      tokenize_number(&anchor, table_info, char_table);
      break;
    case C_LETTER:
      tokenize_identifier(&anchor, table_info, char_table);
      break;
    case C_SYMBOL:
      tokenize_symbol(&anchor, table_info);
      break;
    case C_STRING:
      tokenize_string(&anchor, table_info, char_table);
      break;
    default: {
      fprintf(stderr, "Unknown character encountered in source buffer: %c\n",
              char_table[(unsigned char)*anchor]);
      exit(EXIT_FAILURE);
    }
    }
  }
  // print_tokenizer_xml(out, table_info);
}

void tokenize_identifier(char **anchor, TokenTable *table,
                         CharType *char_table) {
  char *scout = *anchor + 1;
  while ((char_table[(unsigned char)*scout] == C_LETTER ||
          char_table[(unsigned char)*scout] == C_DIGIT) &&
         *scout != '\0')
    scout++;

  if (*scout == '\0') {
    fprintf(stderr, "CATASTROPHIC ERROR: UNEXPECTED END OF FILE WHILE "
                    "PROCESSING A KEYWORD/IDENTIFIER\n");
    exit(EXIT_FAILURE);
  }

  size_t tkn_length = scout - *anchor;
  strncpy(tmp_tkn, *anchor, tkn_length);
  tmp_tkn[tkn_length] = '\0';
  // check against keyword array
  void *result =
      bsearch(tmp_tkn, keywords, sizeof(keywords) / sizeof(keywords[0]),
              sizeof(char *), string_compare);
  if (!result) {
    Token t = (Token){T_IDENTIFIER, *anchor, tkn_length};
    add_token(table, t);
  } else {
    Token t = (Token){T_KEYWORD, *anchor, tkn_length};
    add_token(table, t);
  }
  *anchor = scout;
}

void tokenize_number(char **anchor, TokenTable *table, CharType *char_table) {
  char *scout = *anchor;
  while (char_table[(unsigned char)*scout] == C_DIGIT && *scout != '\0')
    scout++;

  if (*scout == '\0') {
    fprintf(stderr, "CATASTROPHIC ERROR: UNEXPECTED END OF FILE WHILE "
                    "PROCESSING A NUMBER\n");
    exit(EXIT_FAILURE);
  }

  // add error message is non-digit character is encountered
  //  print string till offending character is encountered
  //  print from *anchor to the offending character scanned by *scout
  // if (char_table[(unsigned char)*scout] != C_DIGIT &&
  //     !(isspace((unsigned char)*scout))) {
  //   int len = scout - *anchor;
  //   fprintf(stderr, "Error while scanning number: '%.*s'\n", len, *anchor);
  //   exit(EXIT_FAILURE);
  // }

  size_t tkn_length = scout - *anchor;
  int out_of_range = string_to_int(*anchor, tkn_length);
  if (!out_of_range) {
    Token t = (Token){T_NUMBER, *anchor, tkn_length};
    add_token(table, t);
  } else {
    fprintf(stderr, "number out of range\n");
    exit(EXIT_FAILURE);
  }
  *anchor = scout;
}

void tokenize_symbol(char **anchor, TokenTable *table) {
  Token t = (Token){T_SYMBOL, *anchor, 1};
  add_token(table, t);
  (*anchor)++;
}

void tokenize_string(char **anchor, TokenTable *table, CharType *char_table) {
  char *scout = *anchor + 1;
  (*anchor)++;
  while (char_table[(unsigned char)*scout] != C_STRING && *scout != '\0' &&
         *scout != '\n')
    scout++;

  if (*scout == '\0') {
    fprintf(stderr, "CATASTROPHIC ERROR: UNEXPECTED END OF FILE\n");
    exit(EXIT_FAILURE);
  }

  if (*scout == '\n') {
    fprintf(stderr, "Escape character: %c encountered\n in string constant\n",
            *scout);
    exit(EXIT_FAILURE);
  }

  size_t tkn_length = scout - *anchor;
  Token t = (Token){T_STRING, *anchor, tkn_length};
  add_token(table, t);
  *anchor = scout + 1;
}

// void tokenize_eof(TokenTable* table, char* anchor) {
//     Token eof = (Token) { T_EOF, anchor, 1 };
//     add_token(table, eof);
// }

void handle_comments(char **anchor) {
  // two variants: //, /* to */ and /** to */
  // handles // comment type
  if (**anchor == '/' && *(*anchor + 1) == '/') {
    *anchor += 2;
    while (**anchor != '\n' && **anchor != '\0')
      (*anchor)++;
    if (**anchor == '\n')
      (*anchor)++;
  }

  // handles /* to */ type
  if (**anchor == '/' && *(*anchor + 1) == '*') {
    *anchor += 2;
    while (**anchor != '\0') {
      if (**anchor == '*' && *(*anchor + 1) == '/') {
        *anchor += 2;
        break;
      }
      (*anchor)++;
    }
  }
}

int string_compare(const void *s1, const void *s2) {
  char *tkn = (char *)s1;
  char *keyword_value = *(char **)s2;

  return strcmp(tkn, keyword_value);
}

void add_token(TokenTable *table, Token token) {
  if (table->count >= table->capacity) {
    size_t new_capacity = table->capacity * 2;
    Token *new_pointer = realloc(table->tokens, sizeof(Token) * new_capacity);
    if (!new_pointer) {
      fprintf(stderr, "Fatal error allocating token table\n");
      exit(EXIT_FAILURE);
    }
    table->tokens = new_pointer;
    table->capacity = new_capacity;
  }
  table->tokens[table->count++] = token;
}

int string_to_int(const char *str, size_t size) {
  strncpy(tmp_tkn, str, size);
  tmp_tkn[size] = '\0';
  long value = atol(tmp_tkn);
  if (value > 32767)
    return 1;
  return 0;
}

void print_tokenizer_xml(FILE *out, TokenTable *table) {
  const char *const tag_names[] = {[T_KEYWORD] = "keyword",
                                   [T_SYMBOL] = "symbol",
                                   [T_NUMBER] = "integerConstant",
                                   [T_STRING] = "stringConstant",
                                   [T_IDENTIFIER] = "identifier"};

  fprintf(out, "<tokens>\n");
  for (int i = 0; i < table->count; i++) {
    Token t = table->tokens[i];

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
        fprintf(out, "<symbol> %s </symbol>\n", escaped);
        continue;
      }
    }

    // Fast path for everything else
    fprintf(out, "<%s> %.*s </%s>\n", tag_names[t.type], t.length, t.lexeme,
            tag_names[t.type]);
  }
  fprintf(out, "</tokens>\n");
}
