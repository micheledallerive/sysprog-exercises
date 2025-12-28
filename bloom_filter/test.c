#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../greatest.h"
#include "custom_tests.h"
#include "lib.h"

TEST bitset_create_and_size() {
  bitset_t *bs = bitset_create(100);
  ASSERT(bs != NULL);
  ASSERT_EQ(100, bitset_size(bs));
  bitset_free(bs);
  PASS();
}

TEST bitset_initialization_is_zero() {
  bitset_t *bs = bitset_create(64);
  for (size_t i = 0; i < 64; i++) {
    ASSERT_FALSE(bitset_get(bs, i));
  }
  bitset_free(bs);
  PASS();
}

TEST bitset_set_and_get_basic() {
  bitset_t *bs = bitset_create(10);
  size_t index = rand() % 10;
  bitset_set(bs, index, true);
  ASSERT(bitset_get(bs, index));
  for (size_t i = 0; i < 10; i++) {
    if (i != index) {
      ASSERT_FALSE(bitset_get(bs, i));
    }
  }
  bitset_free(bs);
  PASS();
}

TEST bitset_set_to_false() {
  bitset_t *bs = bitset_create(10);
  bitset_set(bs, 3, true);
  ASSERT(bitset_get(bs, 3));
  bitset_set(bs, 3, false);
  ASSERT_FALSE(bitset_get(bs, 3));
  bitset_free(bs);
  PASS();
}

TEST bitset_clear_all() {
  bitset_t *bs = bitset_create(128);
  for (size_t i = 0; i < 128; i++) {
    bitset_set(bs, i, true);
  }
  bitset_clear(bs);
  for (size_t i = 0; i < 128; i++) {
    ASSERT_FALSE(bitset_get(bs, i));
  }
  bitset_free(bs);
  PASS();
}

TEST bitset_boundary_cases() {
  bitset_t *bs = bitset_create(1);
  bitset_set(bs, 0, true);
  ASSERT(bitset_get(bs, 0));
  bitset_free(bs);

  bs = bitset_create(8);
  bitset_set(bs, 7, true);
  ASSERT(bitset_get(bs, 7));
  bitset_free(bs);

  bs = bitset_create(9);
  bitset_set(bs, 8, true);
  ASSERT(bitset_get(bs, 8));
  bitset_free(bs);
  PASS();
}

TEST bitset_large_indices() {
  size_t size = 10000;
  bitset_t *bs = bitset_create(size);
  bitset_set(bs, 9999, true);
  ASSERT(bitset_get(bs, 9999));
  ASSERT_FALSE(bitset_get(bs, 0));
  bitset_free(bs);
  PASS();
}

TEST bitset_overwrite_same_value() {
  bitset_t *bs = bitset_create(8);
  bitset_set(bs, 4, true);
  bitset_set(bs, 4, true);
  ASSERT(bitset_get(bs, 4));
  bitset_set(bs, 4, false);
  bitset_set(bs, 4, false);
  ASSERT_FALSE(bitset_get(bs, 4));
  bitset_free(bs);
  PASS();
}

TEST bitset_multi_byte_alignment() {
  bitset_t *bs = bitset_create(17);
  bitset_set(bs, 0, true);
  bitset_set(bs, 8, true);
  bitset_set(bs, 16, true);
  ASSERT(bitset_get(bs, 0));
  ASSERT(bitset_get(bs, 8));
  ASSERT(bitset_get(bs, 16));
  bitset_free(bs);
  PASS();
}

TEST bitset_zero_size() {
  bitset_t *bs = bitset_create(0);
  ASSERT(bs != NULL);
  ASSERT_EQ(0, bitset_size(bs));
  bitset_free(bs);
  PASS();
}

TEST bloom_create_and_free() {
  bloom_filter_t *bf = bloom_filter_create(1000);
  ASSERT(bf != NULL);
  bloom_filter_free(bf);
  PASS();
}

TEST bloom_empty_contains_nothing() {
  bloom_filter_t *bf = bloom_filter_create(1000);
  const char *data = "test";
  ASSERT_FALSE(bloom_filter_contains(bf, data, strlen(data)));
  bloom_filter_free(bf);
  PASS();
}

TEST bloom_add_and_contains_single() {
  bloom_filter_t *bf = bloom_filter_create(1000);
  uint64_t item = rand();
  bloom_filter_add(bf, &item, sizeof(item));
  ASSERT(bloom_filter_contains(bf, &item, sizeof(item)));
  bloom_filter_free(bf);
  PASS();
}

TEST bloom_add_and_contains_multiple() {
  bloom_filter_t *bf = bloom_filter_create(1000);
  const char *items[] = {"apple", "orange", "banana", "grape"};
  for (int i = 0; i < 4; i++) {
    bloom_filter_add(bf, items[i], strlen(items[i]));
  }
  for (int i = 0; i < 4; i++) {
    ASSERT(bloom_filter_contains(bf, items[i], strlen(items[i])));
  }
  bloom_filter_free(bf);
  PASS();
}

TEST bloom_integer_keys() {
  bloom_filter_t *bf = bloom_filter_create(500);
  for (uint32_t i = 0; i < 50; i++) {
    bloom_filter_add(bf, &i, sizeof(i));
  }
  for (uint32_t i = 0; i < 50; i++) {
    ASSERT(bloom_filter_contains(bf, &i, sizeof(i)));
  }
  bloom_filter_free(bf);
  PASS();
}

TEST bloom_false_positive_rate() {
  size_t size = 1024;
  bloom_filter_t *bf = bloom_filter_create(size);
  int items_added = 200;
  for (int i = 0; i < items_added; i++) {
    bloom_filter_add(bf, &i, sizeof(i));
  }

  int false_positives = 0;
  int tests = 1000000;
  for (int i = items_added; i < items_added + tests; i++) {
    if (bloom_filter_contains(bf, &i, sizeof(i))) {
      false_positives++;
    }
  }

  // Theoretical false positive rate:
  // (1 - e^(-k*n/m))^k
  // where k = number of hash functions (8)
  //       n = number of inserted items (200)
  //       m = size of bit array (1024)
  // This gives approximately 0.1523

  double rate = (double)false_positives / tests;
  ASSERT(rate > 0.14 && rate < 0.17);
  bloom_filter_free(bf);
  PASS();
}

TEST bloom_false_negative_rate() {
  bloom_filter_t *bf = bloom_filter_create(1000);
  const char *items[] = {"one", "two", "three", "four", "five"};
  for (int i = 0; i < 5; i++) {
    bloom_filter_add(bf, items[i], strlen(items[i]));
  }
  for (int i = 0; i < 5; i++) {
    ASSERT(bloom_filter_contains(bf, items[i], strlen(items[i])));
  }
  bloom_filter_free(bf);
  PASS();
}

TEST bloom_collision_behavior() {
  bloom_filter_t *bf = bloom_filter_create(8);
  const char *a = "a";
  const char *b = "b";
  bloom_filter_add(bf, a, 1);
  bloom_filter_add(bf, b, 1);
  ASSERT(bloom_filter_contains(bf, a, 1));
  ASSERT(bloom_filter_contains(bf, b, 1));
  bloom_filter_free(bf);
  PASS();
}

TEST bloom_different_sizes_same_data() {
  const char *data = "consistency";
  bloom_filter_t *bf1 = bloom_filter_create(100);
  bloom_filter_t *bf2 = bloom_filter_create(10000);

  bloom_filter_add(bf1, data, strlen(data));
  bloom_filter_add(bf2, data, strlen(data));

  ASSERT(bloom_filter_contains(bf1, data, strlen(data)));
  ASSERT(bloom_filter_contains(bf2, data, strlen(data)));

  bloom_filter_free(bf1);
  bloom_filter_free(bf2);
  PASS();
}

TEST bloom_null_bytes_in_data() {
  bloom_filter_t *bf = bloom_filter_create(100);
  char data[4] = {0, 1, 0, 2};
  bloom_filter_add(bf, data, 4);
  ASSERT(bloom_filter_contains(bf, data, 4));
  bloom_filter_free(bf);
  PASS();
}

TEST bloom_large_data_blob() {
  bloom_filter_t *bf = bloom_filter_create(1000);
  uint8_t large[1024];
  for (int i = 0; i < 1024; i++) large[i] = (uint8_t)(i % 256);
  bloom_filter_add(bf, large, 1024);
  ASSERT(bloom_filter_contains(bf, large, 1024));
  bloom_filter_free(bf);
  PASS();
}

TEST bloom_add_and_contains_many() {
  bloom_filter_t *bf = bloom_filter_create(20000000);
  const int num_items = 1000000;
  for (int i = 0; i < num_items; i++) {
    bloom_filter_add(bf, &i, sizeof(i));
  }
  for (int i = 0; i < num_items; i++) {
    ASSERT(bloom_filter_contains(bf, &i, sizeof(i)));
  }
  bloom_filter_free(bf);
  PASS();
}

SUITE(bitset_suite) {
  RUN_TEST(bitset_create_and_size);
  RUN_TEST(bitset_initialization_is_zero);
  RUN_TEST(bitset_set_and_get_basic);
  RUN_TEST(bitset_set_to_false);
  RUN_TEST(bitset_clear_all);
  RUN_TEST(bitset_boundary_cases);
  RUN_TEST(bitset_large_indices);
  RUN_TEST(bitset_overwrite_same_value);
  RUN_TEST(bitset_multi_byte_alignment);
  RUN_TEST(bitset_zero_size);

  for (int i = 0; i < 10; i++) {
    RUN_TEST(bitset_set_and_get_basic);
  }
}

SUITE(bloom_filter_suite) {
  RUN_TEST(bloom_create_and_free);
  RUN_TEST(bloom_empty_contains_nothing);
  RUN_TEST(bloom_add_and_contains_single);
  RUN_TEST(bloom_add_and_contains_multiple);
  RUN_TEST(bloom_integer_keys);
  RUN_TEST(bloom_false_positive_rate);
  RUN_TEST(bloom_false_negative_rate);
  RUN_TEST(bloom_collision_behavior);
  RUN_TEST(bloom_different_sizes_same_data);
  RUN_TEST(bloom_null_bytes_in_data);
  RUN_TEST(bloom_large_data_blob);

  for (int i = 0; i < 8; i++) {
    RUN_TEST(bloom_add_and_contains_single);
  }
  RUN_TEST(bloom_add_and_contains_many);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  srand(42);
  GREATEST_MAIN_BEGIN();

  RUN_SUITE(bitset_suite);
  RUN_SUITE(bloom_filter_suite);

  GREATEST_PRINT_REPORT();
  custom_tests();
  return greatest_all_passed() ? EXIT_SUCCESS : EXIT_FAILURE;
}
