#ifndef LIB_H
#define LIB_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/**
 * Encode the given values using LEB128 encoding.
 * Write the resulting bytes to the output buffer, and set output_size to the
 * number of bytes written. You can assume that the output buffer is large
 * enough to hold the encoded data.
 */
void leb128_encode(const uint64_t *values, const size_t count, uint8_t *output,
                   size_t *output_size);
/**
 * Decode the given LEB128-encoded input bytes.
 * Write the resulting values to the values array, and set values_count to the
 * number of values decoded. You can assume that the values array is large
 * enough to hold the decoded data.
 */
void leb128_decode(const uint8_t *input, const size_t input_size,
                   uint64_t *values, size_t *values_count);

#endif  // LIB_H
