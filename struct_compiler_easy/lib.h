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
};

struct field {
  enum type type;
  size_t offset;
};

/**
 * Implement alignment and offset logic to compute the total size of a struct
 * and the offset of each field.
 */
size_t compile(size_t num_fields, struct field *fields);

#endif  // LIB_H
