#ifndef LIB_H
#define LIB_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef enum {
  FP_NAN,
  FP_INFINITE,
  FP_NORMAL,
  FP_SUBNORMAL,
  FP_ZERO
} fp_class_t;

fp_class_t fpclassify(float value);

#endif  // LIB_H
