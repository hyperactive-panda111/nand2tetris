#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stddef.h>
#define TABLE_SIZE 64

typedef enum { T_FIELD, T_STATIC, T_ARG, T_VAR } T_KIND;

typedef struct {
  char *name;
  size_t name_length;
  char *type;
  size_t type_length;
  T_KIND kind;
  int index;
} SymbolTableEntry;

typedef struct {
  SymbolTableEntry entries[64];
  int count;
  int field_count;
  int static_count;
} ClassTable;

typedef struct {
  SymbolTableEntry entries[64];
  int count;
  int arg_count;
  int local_count;
} SubTable;

void add_to_class_table(ClassTable *table, char *name, int name_len, char *type,
                        int type_name_len, T_KIND kind);
void add_to_sub_table(SubTable *table, char *name, int name_len, char *type,
                      int type_name_len, T_KIND kind);
SymbolTableEntry *in_sub_table(SubTable *, char *name, int name_length);
SymbolTableEntry *in_class_table(ClassTable *, char *name, int name_length);
void init_class_table(ClassTable* table);
void init_sub_table(SubTable* table);

#endif
