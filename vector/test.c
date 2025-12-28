#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../greatest.h"
#include "custom_tests.h"
#include "lib.h"

TEST test_size_empty() {
  vector_t *v = vector_create(0);
  ASSERT_EQ(0, vector_size(v));
  vector_free(v);
  PASS();
}

TEST test_size_after_push() {
  vector_t *v = vector_create(2);
  vector_push_back(v, 1);
  vector_push_back(v, 2);
  ASSERT_EQ(2, vector_size(v));
  vector_free(v);
  PASS();
}

TEST test_size_after_pop() {
  vector_t *v = vector_create(2);
  vector_push_back(v, 1);
  vector_push_back(v, 2);
  vector_pop_back(v);
  ASSERT_EQ(1, vector_size(v));
  vector_free(v);
  PASS();
}

SUITE(suite_vector_size) {
  RUN_TEST(test_size_empty);
  RUN_TEST(test_size_after_push);
  RUN_TEST(test_size_after_pop);
}

TEST test_empty_true() {
  vector_t *v = vector_create(0);
  ASSERT(vector_empty(v));
  vector_free(v);
  PASS();
}

TEST test_empty_false() {
  vector_t *v = vector_create(1);
  vector_push_back(v, 10);
  ASSERT(!vector_empty(v));
  vector_free(v);
  PASS();
}

TEST test_empty_after_clear() {
  vector_t *v = vector_create(4);
  vector_push_back(v, 1);
  vector_clear(v);
  ASSERT(vector_empty(v));
  vector_free(v);
  PASS();
}

SUITE(suite_vector_empty) {
  RUN_TEST(test_empty_true);
  RUN_TEST(test_empty_false);
  RUN_TEST(test_empty_after_clear);
}

TEST test_front_back_single() {
  vector_t *v = vector_create(1);
  vector_push_back(v, 42);
  ASSERT_EQ(42, vector_front(v));
  ASSERT_EQ(42, vector_back(v));
  vector_free(v);
  PASS();
}

TEST test_front_back_multiple() {
  vector_t *v = vector_create(3);
  vector_push_back(v, 1);
  vector_push_back(v, 2);
  vector_push_back(v, 3);
  ASSERT_EQ(1, vector_front(v));
  ASSERT_EQ(3, vector_back(v));
  vector_free(v);
  PASS();
}

TEST test_at_access() {
  vector_t *v = vector_create(5);
  for (int i = 0; i < 5; i++) vector_push_back(v, i * 10);
  for (int i = 0; i < 5; i++) ASSERT_EQ(i * 10, vector_at(v, i));
  vector_free(v);
  PASS();
}

TEST test_data_pointer() {
  vector_t *v = vector_create(3);
  vector_push_back(v, 5);
  vector_push_back(v, 10);
  int *d = vector_data(v);
  ASSERT_EQ(5, d[0]);
  ASSERT_EQ(10, d[1]);
  vector_free(v);
  PASS();
}

SUITE(suite_vector_accessors) {
  RUN_TEST(test_front_back_single);
  RUN_TEST(test_front_back_multiple);
  RUN_TEST(test_at_access);
  RUN_TEST(test_data_pointer);
}

TEST test_capacity_initial() {
  vector_t *v = vector_create(10);
  ASSERT_EQ(10, vector_capacity(v));
  vector_free(v);
  PASS();
}

TEST test_reserve_grow() {
  vector_t *v = vector_create(2);
  vector_reserve(v, 10);
  ASSERT_EQ(10, vector_capacity(v));
  vector_free(v);
  PASS();
}

TEST test_reserve_no_shrink() {
  vector_t *v = vector_create(10);
  vector_reserve(v, 5);
  ASSERT_EQ(10, vector_capacity(v));
  vector_free(v);
  PASS();
}

TEST test_shrink_to_fit() {
  vector_t *v = vector_create(100);
  for (int i = 0; i < 10; i++) vector_push_back(v, i);
  vector_shrink_to_fit(v);
  ASSERT_EQ(10, vector_capacity(v));
  ASSERT_EQ(10, vector_size(v));
  vector_free(v);
  PASS();
}

TEST test_resize_grow_with_default() {
  vector_t *v = vector_create(2);
  vector_resize(v, 5, 7);
  ASSERT_EQ(5, vector_size(v));
  for (size_t i = 0; i < 5; i++) ASSERT_EQ(7, vector_at(v, i));
  vector_free(v);
  PASS();
}

TEST test_resize_shrink() {
  vector_t *v = vector_create(5);
  for (int i = 0; i < 5; i++) vector_push_back(v, i);
  vector_resize(v, 2, 0);
  ASSERT_EQ(2, vector_size(v));
  ASSERT_EQ(0, vector_at(v, 0));
  ASSERT_EQ(1, vector_at(v, 1));
  vector_free(v);
  PASS();
}

SUITE(suite_vector_capacity) {
  RUN_TEST(test_capacity_initial);
  RUN_TEST(test_reserve_grow);
  RUN_TEST(test_reserve_no_shrink);
  RUN_TEST(test_shrink_to_fit);
  RUN_TEST(test_resize_grow_with_default);
  RUN_TEST(test_resize_shrink);
}

TEST test_push_back_growth() {
  vector_t *v = vector_create(1);
  for (int i = 0; i < 10; i++) {
    vector_push_back(v, i);
  }
  ASSERT_EQ(10, vector_size(v));
  ASSERT_EQ(9, vector_back(v));
  vector_free(v);
  PASS();
}

TEST test_pop_back_single() {
  vector_t *v = vector_create(1);
  vector_push_back(v, 5);
  vector_pop_back(v);
  ASSERT(vector_empty(v));
  vector_free(v);
  PASS();
}

TEST test_pop_back_multiple() {
  vector_t *v = vector_create(3);
  vector_push_back(v, 1);
  vector_push_back(v, 2);
  vector_push_back(v, 3);
  vector_pop_back(v);
  ASSERT_EQ(2, vector_back(v));
  vector_free(v);
  PASS();
}

SUITE(suite_vector_push_pop) {
  RUN_TEST(test_push_back_growth);
  RUN_TEST(test_pop_back_single);
  RUN_TEST(test_pop_back_multiple);
}

TEST test_insert_middle() {
  vector_t *v = vector_create(5);
  vector_push_back(v, 1);
  vector_push_back(v, 3);
  vector_insert_before(v, 1, 2);
  ASSERT_EQ(3, vector_size(v));
  ASSERT_EQ(2, vector_at(v, 1));
  vector_free(v);
  PASS();
}

TEST test_insert_front() {
  vector_t *v = vector_create(3);
  vector_push_back(v, 2);
  vector_push_back(v, 3);
  vector_insert_before(v, 0, 1);
  ASSERT_EQ(1, vector_front(v));
  vector_free(v);
  PASS();
}

TEST test_insert_end_index() {
  vector_t *v = vector_create(3);
  vector_push_back(v, 1);
  vector_push_back(v, 2);
  vector_insert_before(v, 2, 3);
  ASSERT_EQ(3, vector_size(v));
  ASSERT_EQ(3, vector_back(v));
  vector_free(v);
  PASS();
}

TEST test_erase_middle() {
  vector_t *v = vector_create(5);
  for (int i = 0; i < 5; i++) vector_push_back(v, i);
  vector_erase(v, 2);
  ASSERT_EQ(4, vector_size(v));
  ASSERT_EQ(3, vector_at(v, 2));
  vector_free(v);
  PASS();
}

TEST test_erase_front() {
  vector_t *v = vector_create(3);
  vector_push_back(v, 1);
  vector_push_back(v, 2);
  vector_erase(v, 0);
  ASSERT_EQ(2, vector_front(v));
  vector_free(v);
  PASS();
}

TEST test_erase_back() {
  vector_t *v = vector_create(3);
  vector_push_back(v, 1);
  vector_push_back(v, 2);
  vector_erase(v, 1);
  ASSERT_EQ(1, vector_back(v));
  vector_free(v);
  PASS();
}

SUITE(suite_vector_insert_erase) {
  RUN_TEST(test_insert_middle);
  RUN_TEST(test_insert_front);
  RUN_TEST(test_insert_end_index);
  RUN_TEST(test_erase_middle);
  RUN_TEST(test_erase_front);
  RUN_TEST(test_erase_back);
}

TEST test_find_and_contains() {
  vector_t *v = vector_create(5);
  vector_push_back(v, 10);
  vector_push_back(v, 20);
  vector_push_back(v, 30);
  ASSERT_EQ(1, vector_find(v, 20));
  ASSERT_EQ(-1, vector_find(v, 40));
  ASSERT(vector_contains(v, 10));
  ASSERT(!vector_contains(v, 50));
  vector_free(v);
  PASS();
}

SUITE(suite_vector_search) {
  RUN_TEST(test_find_and_contains);
}

TEST test_sort_basic() {
  vector_t *v = vector_create(5);
  vector_push_back(v, 50);
  vector_push_back(v, 10);
  vector_push_back(v, 40);
  vector_push_back(v, 20);
  vector_push_back(v, 30);
  vector_sort(v);
  for (int i = 0; i < 5; i++) {
    ASSERT_EQ((i + 1) * 10, vector_at(v, i));
  }
  vector_free(v);
  PASS();
}

TEST test_sort_empty_and_single() {
  vector_t *v = vector_create(1);
  vector_sort(v);
  ASSERT_EQ(0, vector_size(v));
  vector_push_back(v, 100);
  vector_sort(v);
  ASSERT_EQ(100, vector_at(v, 0));
  vector_free(v);
  PASS();
}

SUITE(suite_vector_sort) {
  RUN_TEST(test_sort_basic);
  RUN_TEST(test_sort_empty_and_single);
}

TEST test_reverse_even() {
  vector_t *v = vector_create(4);
  vector_push_back(v, 1);
  vector_push_back(v, 2);
  vector_push_back(v, 3);
  vector_push_back(v, 4);
  vector_reverse(v);
  ASSERT_EQ(4, vector_at(v, 0));
  ASSERT_EQ(3, vector_at(v, 1));
  ASSERT_EQ(2, vector_at(v, 2));
  ASSERT_EQ(1, vector_at(v, 3));
  vector_free(v);
  PASS();
}

TEST test_reverse_odd() {
  vector_t *v = vector_create(3);
  vector_push_back(v, 10);
  vector_push_back(v, 20);
  vector_push_back(v, 30);
  vector_reverse(v);
  ASSERT_EQ(30, vector_at(v, 0));
  ASSERT_EQ(20, vector_at(v, 1));
  ASSERT_EQ(10, vector_at(v, 2));
  vector_free(v);
  PASS();
}

TEST test_reverse_empty_and_single() {
  vector_t *v = vector_create(2);
  vector_reverse(v);
  ASSERT_EQ(0, vector_size(v));
  vector_push_back(v, 42);
  vector_reverse(v);
  ASSERT_EQ(42, vector_at(v, 0));
  vector_free(v);
  PASS();
}

SUITE(suite_vector_reverse) {
  RUN_TEST(test_reverse_even);
  RUN_TEST(test_reverse_odd);
  RUN_TEST(test_reverse_empty_and_single);
}

TEST test_clear_empty() {
  vector_t *v = vector_create(0);
  vector_clear(v);
  ASSERT(vector_empty(v));
  vector_free(v);
  PASS();
}

TEST test_clear_non_empty() {
  vector_t *v = vector_create(5);
  for (int i = 0; i < 3; i++) vector_push_back(v, i);
  vector_clear(v);
  ASSERT_EQ(0, vector_size(v));
  ASSERT(vector_capacity(v) >= 3);
  vector_free(v);
  PASS();
}

SUITE(suite_vector_clear) {
  RUN_TEST(test_clear_empty);
  RUN_TEST(test_clear_non_empty);
}

TEST test_stress_large_push_pop() {
  vector_t *v = vector_create(1);
  for (int i = 0; i < 10000; i++) vector_push_back(v, i);
  for (int i = 0; i < 5000; i++) vector_pop_back(v);
  ASSERT_EQ(5000, vector_size(v));
  ASSERT_EQ(4999, vector_back(v));
  vector_free(v);
  PASS();
}

TEST test_stress_huge_vector() {
  vector_t *v = vector_create(0);
  const size_t N = 100;
  for (size_t i = 0; i < N; i++) {
    vector_push_back(v, i);
  }
  ASSERT_EQ(N, vector_size(v));
  for (int i = 0; i < (int)N; i++) ASSERT_EQ(i, vector_at(v, i));
  while (!vector_empty(v)) {
    vector_pop_back(v);
  }
  vector_free(v);
  PASS();
}

TEST test_scenario_mixed_ops() {
  vector_t *v = vector_create(2);
  vector_push_back(v, 1);
  vector_push_back(v, 3);
  vector_insert_before(v, 1, 2);
  vector_erase(v, 0);
  ASSERT_EQ(2, vector_front(v));
  ASSERT_EQ(3, vector_back(v));
  vector_free(v);
  PASS();
}

SUITE(suite_vector_stress) {
  RUN_TEST(test_stress_large_push_pop);
  RUN_TEST(test_stress_huge_vector);
  RUN_TEST(test_scenario_mixed_ops);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite_vector_size);
  RUN_SUITE(suite_vector_empty);
  RUN_SUITE(suite_vector_accessors);
  RUN_SUITE(suite_vector_capacity);
  RUN_SUITE(suite_vector_push_pop);
  RUN_SUITE(suite_vector_insert_erase);
  RUN_SUITE(suite_vector_search);
  RUN_SUITE(suite_vector_sort);
  RUN_SUITE(suite_vector_reverse);
  RUN_SUITE(suite_vector_clear);
  RUN_SUITE(suite_vector_stress);
  GREATEST_PRINT_REPORT();
  custom_tests();
  return greatest_all_passed() ? EXIT_SUCCESS : EXIT_FAILURE;
}
