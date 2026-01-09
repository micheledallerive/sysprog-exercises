#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"

void leb128_encode(const uint64_t *values, const size_t count, uint8_t *output,
                   size_t *output_size) {}

void leb128_decode(const uint8_t *input, const size_t input_size,
                   uint64_t *values, size_t *values_count) {}
