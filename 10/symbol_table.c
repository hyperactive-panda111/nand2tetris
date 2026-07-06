#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void reset_subroutine_table(SubTable *table) {
  table->count = 0;
  table->arg_count = 0;
  table->local_count = 0;
}

SymbolTableEntry *in_sub_table(SubTable *table, char *name, int name_length) {
  for (int i = table->count - 1; i >= 0; --i) {
    if ((size_t)name_length != table->entries[i].name_length)
      continue;
    else if (strncmp(name, table->entries[i].name, name_length) == 0)
      return &(table->entries[i]);
  }
  return NULL;
}

SymbolTableEntry *in_class_table(ClassTable *table, char *name,
                                 int name_length) {
  for (int i = table->count - 1; i >= 0; --i) {
    if ((size_t)name_length != table->entries[i].name_length)
      continue;
    else if (strncmp(name, table->entries[i].name, name_length) == 0)
      return &(table->entries[i]);
  }
  return NULL;
}

void add_to_class_table(ClassTable *table, char *name, int name_len, char *type,
                        int type_name_len, T_KIND kind) {
  if (table->count >= TABLE_SIZE) {
    fprintf(stderr, "Table size exceeded\n");
    exit(EXIT_FAILURE);
  }

  SymbolTableEntry *e = &table->entries[table->count++];
  e->name = name;
  e->name_length = name_len;
  e->type = type;
  e->type_length = type_name_len;
  e->kind = kind;
  e->index = (kind == T_FIELD) ? table->field_count++ : table->static_count++;
}

void add_to_sub_table(SubTable *table, char *name, int name_len, char *type,
                      int type_name_len, T_KIND kind) {
  if (table->count >= TABLE_SIZE) {
    fprintf(stderr, "Table size overflow\n");
    exit(EXIT_FAILURE);
  }

  SymbolTableEntry *e = &table->entries[table->count++];
  e->name = name;
  e->name_length = name_len;
  e->type = type;
  e->type_length = type_name_len;
  e->kind = kind;
  e->index = (kind == T_VAR) ? table->local_count++ : table->arg_count++;
}

void init_class_table(ClassTable *table) {
  table->count = 0;
  table->field_count = 0;
  table->static_count = 0;
}

void init_sub_table(SubTable *table) {
  table->count = 0;
  table->arg_count = 0;
  table->local_count = 0;
}
