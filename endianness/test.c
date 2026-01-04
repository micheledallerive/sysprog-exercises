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

TEST test_system_endianness(void) {
  uint32_t check = 0x01020304;
  uint8_t *byte_ptr = (uint8_t *)&check;
  endianness_t expected;

  if (byte_ptr[0] == 0x04) {
    expected = LITTLE_ENDIAN;
  } else if (byte_ptr[0] == 0x01) {
    expected = BIG_ENDIAN;
  } else {
    expected = OTHER_ENDIAN;
  }

  ASSERT_EQ(expected, get_system_endianness());
  PASS();
}

SUITE(detection_suite) {
  RUN_TEST(test_system_endianness);
}

TEST test_swap_u16_basic(void) {
  ASSERT_EQ_FMT(0x3412, swap_u16(0x1234), "0x%04x");
  ASSERT_EQ_FMT(0xBBAA, swap_u16(0xAABB), "0x%04x");
  ASSERT_EQ_FMT(0x0000, swap_u16(0x0000), "0x%04x");
  ASSERT_EQ_FMT(0xFFFF, swap_u16(0xFFFF), "0x%04x");
  PASS();
}

TEST test_swap_u32_basic(void) {
  ASSERT_EQ_FMT(0xDDCCBBAA, swap_u32(0xAABBCCDD), "0x%08x");
  ASSERT_EQ_FMT(0xEFBEADDE, swap_u32(0xDEADBEEF), "0x%08x");
  ASSERT_EQ_FMT(0x00000000, swap_u32(0x00000000), "0x%08x");
  PASS();
}

TEST test_swap_u64_basic(void) {
  uint64_t input = 0x1122334455667788ULL;
  uint64_t expected = 0x8877665544332211ULL;
  ASSERT_EQ_FMT(expected, swap_u64(input), "0x%016lx");
  ASSERT_EQ_FMT((uint64_t)0x0000000000000000ULL, swap_u64(0ULL), "0x%016lx");
  PASS();
}

TEST test_swap_symmetry(void) {
  for (int i = 0; i < 100; i++) {
    uint16_t r16 = (uint16_t)rand();
    ASSERT_EQ_FMT(r16, swap_u16(swap_u16(r16)), "0x%04x");
  }

  for (int i = 0; i < 100; i++) {
    uint32_t r32 = (uint32_t)rand() ^ ((uint32_t)rand() << 16);
    ASSERT_EQ_FMT(r32, swap_u32(swap_u32(r32)), "0x%08x");
  }

  for (int i = 0; i < 100; i++) {
    uint64_t r64 = (uint64_t)rand() | ((uint64_t)rand() << 16) |
                   ((uint64_t)rand() << 32) | ((uint64_t)rand() << 48);
    ASSERT_EQ_FMT(r64, swap_u64(swap_u64(r64)), "0x%016lx");
  }
  PASS();
}

TEST test_random_swap(void) {
  for (size_t i = 0; i < 1000; ++i) {
    uint16_t val16 = (uint16_t)rand();
    uint16_t swapped16 = swap_u16(val16);
    uint16_t expected16 = __builtin_bswap16(val16);
    ASSERT_EQ_FMT(expected16, swapped16, "0x%04x");
    ASSERT_EQ_FMT(val16, swap_u16(swapped16), "0x%04x");

    uint32_t val32 = (uint32_t)rand() | ((uint32_t)rand() << 16);
    uint32_t swapped32 = swap_u32(val32);
    uint32_t expected32 = __builtin_bswap32(val32);
    ASSERT_EQ_FMT(expected32, swapped32, "0x%08x");
    ASSERT_EQ_FMT(val32, swap_u32(swapped32), "0x%08x");

    uint64_t val64 = (uint64_t)rand() | ((uint64_t)rand() << 16) |
                     ((uint64_t)rand() << 32) | ((uint64_t)rand() << 48);
    uint64_t swapped64 = swap_u64(val64);
    uint64_t expected64 = __builtin_bswap64(val64);
    ASSERT_EQ_FMT(expected64, swapped64, "0x%016lx");
    ASSERT_EQ_FMT(val64, swap_u64(swapped64), "0x%016lx");
  }
  PASS();
}

SUITE(conversion_suite) {
  RUN_TEST(test_swap_u16_basic);
  RUN_TEST(test_swap_u32_basic);
  RUN_TEST(test_swap_u64_basic);
  RUN_TEST(test_swap_symmetry);
  RUN_TEST(test_random_swap);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  srand(42);
  GREATEST_MAIN_BEGIN();

  RUN_SUITE(detection_suite);
  RUN_SUITE(conversion_suite);

  GREATEST_PRINT_REPORT();
  custom_tests();
  return greatest_all_passed() ? EXIT_SUCCESS : EXIT_FAILURE;
}
