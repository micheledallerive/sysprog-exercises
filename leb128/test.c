#include <assert.h>
#include <float.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../greatest.h"
#include "custom_tests.h"
#include "lib.h"

void custom_tests(void);

TEST test_encode_zero(void) {
  uint64_t val = 0;
  uint8_t output[16];
  size_t size = 0;

  leb128_encode(&val, 1, output, &size);

  ASSERT_EQ(1, size);
  ASSERT_EQ(0x00, output[0]);
  PASS();
}

TEST test_encode_127(void) {
  uint64_t val = 127;
  uint8_t output[16];
  size_t size = 0;

  leb128_encode(&val, 1, output, &size);

  ASSERT_EQ(1, size);
  ASSERT_EQ(0x7F, output[0]);
  PASS();
}

TEST test_encode_128(void) {
  uint64_t val = 128;
  uint8_t output[16];
  size_t size = 0;

  leb128_encode(&val, 1, output, &size);

  ASSERT_EQ(2, size);
  ASSERT_EQ(0x80, output[0]);
  ASSERT_EQ(0x01, output[1]);
  PASS();
}

TEST test_encode_u64_max(void) {
  uint64_t val = UINT64_MAX;
  uint8_t output[16];
  size_t size = 0;

  leb128_encode(&val, 1, output, &size);

  ASSERT_EQ(10, size);
  for (int i = 0; i < 9; i++) {
    ASSERT_EQ(0xFF, output[i]);
  }
  ASSERT_EQ(0x01, output[9]);
  PASS();
}

TEST test_decode_simple(void) {
  uint8_t input[] = {0xE5, 0x8E, 0x26};
  uint64_t values[1];
  size_t count = 0;

  leb128_decode(input, sizeof(input), values, &count);

  ASSERT_EQ(1, count);
  ASSERT_EQ(624485, values[0]);
  PASS();
}

TEST test_roundtrip_single_values(void) {
  uint64_t inputs[] = {1,   42,     127,        128,
                       255, 123456, UINT32_MAX, UINT64_MAX - 1};

  for (size_t i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++) {
    uint64_t original = inputs[i];
    uint8_t buffer[16];
    size_t enc_size = 0;

    leb128_encode(&original, 1, buffer, &enc_size);

    uint64_t decoded = 0;
    size_t dec_count = 0;
    leb128_decode(buffer, enc_size, &decoded, &dec_count);

    ASSERT_EQ(1, dec_count);
    ASSERT_EQ(original, decoded);
  }
  PASS();
}

TEST test_roundtrip_multiple_values(void) {
  uint64_t originals[] = {10, 200, 3000, 40000, 500000};
  size_t count = 5;
  uint8_t buffer[100];
  size_t enc_size = 0;

  leb128_encode(originals, count, buffer, &enc_size);

  uint64_t decoded[5];
  size_t dec_count = 0;

  leb128_decode(buffer, enc_size, decoded, &dec_count);

  ASSERT_EQ(count, dec_count);
  for (size_t i = 0; i < count; i++) {
    ASSERT_EQ(originals[i], decoded[i]);
  }
  PASS();
}

TEST test_encode_buffer_concatenation(void) {
  uint64_t v1 = 0xAA;
  uint64_t v2 = 0xBB;
  uint8_t buffer[16];
  size_t size = 0;
  uint64_t vals[] = {v1, v2};

  leb128_encode(vals, 2, buffer, &size);

  ASSERT(size > 2);

  uint8_t b1 = 0xAA | 0x80;
  uint8_t b2 = 0x01;
  ASSERT_EQ(b1, buffer[0]);
  ASSERT_EQ(b2, buffer[1]);

  uint8_t b3 = 0xBB | 0x80;
  uint8_t b4 = 0x01;
  ASSERT_EQ(b3, buffer[2]);
  ASSERT_EQ(b4, buffer[3]);
  PASS();
}

TEST test_random_fuzz(void) {
  for (int i = 0; i < 100; i++) {
    uint64_t r = ((uint64_t)rand() << 32) | rand();
    uint8_t buf[16];
    size_t sz = 0;

    leb128_encode(&r, 1, buf, &sz);

    uint64_t out = 0;
    size_t cnt = 0;
    leb128_decode(buf, sz, &out, &cnt);

    ASSERT_EQ(r, out);
  }
  PASS();
}

TEST test_empty_input(void) {
  uint64_t val = 123;
  uint8_t buf[10];
  size_t sz = 0;

  leb128_encode(&val, 0, buf, &sz);
  ASSERT_EQ(0, sz);

  size_t count = 0;
  leb128_decode(buf, 0, &val, &count);
  ASSERT_EQ(0, count);

  PASS();
}

TEST test_boundary_14_bit(void) {
  uint64_t val = 16383;
  uint8_t buf[10];
  size_t sz = 0;
  leb128_encode(&val, 1, buf, &sz);
  ASSERT_EQ(2, sz);
  ASSERT_EQ(0xFF, buf[0]);
  ASSERT_EQ(0x7F, buf[1]);

  uint64_t decoded;
  size_t cnt;
  leb128_decode(buf, sz, &decoded, &cnt);
  ASSERT_EQ(val, decoded);
  PASS();
}

TEST test_boundary_14_bit_overflow(void) {
  uint64_t val = 16384;
  uint8_t buf[10];
  size_t sz = 0;
  leb128_encode(&val, 1, buf, &sz);
  ASSERT_EQ(3, sz);
  ASSERT_EQ(0x80, buf[0]);
  ASSERT_EQ(0x80, buf[1]);
  ASSERT_EQ(0x01, buf[2]);
  PASS();
}

SUITE(leb128_suite) {
  RUN_TEST(test_encode_zero);
  RUN_TEST(test_encode_127);
  RUN_TEST(test_encode_128);
  RUN_TEST(test_encode_u64_max);
  RUN_TEST(test_boundary_14_bit);
  RUN_TEST(test_boundary_14_bit_overflow);
  RUN_TEST(test_encode_buffer_concatenation);

  RUN_TEST(test_decode_simple);
  RUN_TEST(test_roundtrip_single_values);
  RUN_TEST(test_roundtrip_multiple_values);
  RUN_TEST(test_random_fuzz);
  RUN_TEST(test_empty_input);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  srand(42);
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(leb128_suite);
  GREATEST_PRINT_REPORT();
  custom_tests();
  return greatest_all_passed() ? EXIT_SUCCESS : EXIT_FAILURE;
}
