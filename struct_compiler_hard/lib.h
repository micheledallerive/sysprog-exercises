#ifndef LIB_H
#define LIB_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

enum type {
  FIELD_TYPE_CHAR,
  FIELD_TYPE_INT,
  FIELD_TYPE_FLOAT,
  FIELD_TYPE_DOUBLE,
  FIELD_TYPE_POINTER,
  FIELD_TYPE_STRUCT,
  FIELD_TYPE_UNION,
  FIELD_TYPE_ARRAY
};

struct field_info;
struct struct_info {
  size_t num_fields;
  struct field_info *fields;
};

struct union_info {
  size_t num_fields;
  struct field_info *fields;
};

struct array_info {
  struct field_info *element_type;
  size_t length;
};

struct field_info {
  enum type type;
  union {
    struct struct_info struct_info;
    struct union_info union_info;
    struct array_info array_info;
  };
  size_t offset;
};

static inline bool is_simple_type(enum type t) {
  return t == FIELD_TYPE_CHAR || t == FIELD_TYPE_INT || t == FIELD_TYPE_FLOAT ||
         t == FIELD_TYPE_DOUBLE || t == FIELD_TYPE_POINTER;
}

static const int MAX_DEPTH = 10;
static const char IDENT[] = "                    ";

static inline const char *get_ident(int depth) {
  assert(depth < MAX_DEPTH);
  assert(sizeof(IDENT) == 20 + 1);
  return IDENT + 20 - 2 * depth;
}

static inline void __print_type(struct field_info *info, int depth) {
  printf("%s", get_ident(depth));
  switch (info->type) {
    case FIELD_TYPE_CHAR:
      printf("char");
      break;
    case FIELD_TYPE_INT:
      printf("int");
      break;
    case FIELD_TYPE_FLOAT:
      printf("float");
      break;
    case FIELD_TYPE_DOUBLE:
      printf("double");
      break;
    case FIELD_TYPE_POINTER:
      printf("void*");
      break;
    case FIELD_TYPE_STRUCT:
      printf("struct {\n");
      for (size_t i = 0; i < info->struct_info.num_fields; ++i) {
        __print_type(&info->struct_info.fields[i], depth + 1);
      }
      printf("%s}", get_ident(depth));
      break;
    case FIELD_TYPE_UNION:
      printf("union {\n");
      for (size_t i = 0; i < info->union_info.num_fields; ++i) {
        __print_type(&info->union_info.fields[i], depth + 1);
      }
      printf("%s}", get_ident(depth));
      break;
    case FIELD_TYPE_ARRAY:
      __print_type(info->array_info.element_type, depth);
      printf("[%zu];", info->array_info.length);
      break;
  }
  printf(";\n");
};
static inline void print_type(struct field_info *info) {
  __print_type(info, 0);
}

// Suggested helper functions:
// size_t get_simple_type_size(enum type t);
// size_t compile_array(struct array_info *ainfo);
// size_t compile_union(struct union_info *uinfo);
// size_t compile_struct(struct struct_info *sinfo);

/**
 * Compile the field info by filling the offset parameter appropriately and
 * returning the total field size.
 * The passed field_info will always be a struct, and the offset of the
 * top-level passed struct will not be tested.
 */
size_t compile(struct field_info *field);

#endif  // LIB_H
