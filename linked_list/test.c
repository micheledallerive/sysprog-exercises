#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "custom_tests.h"
#include "greatest.h"
#include "lib.h"

TEST test_length_empty() {
  list_t *l = list_create();
  ASSERT_EQ(0, list_length(l));
  list_free(l);
  PASS();
}

TEST test_length_single() {
  list_t *l = list_create();
  list_push_back(l, list_node_create(10));
  ASSERT_EQ(1, list_length(l));
  list_free(l);
  PASS();
}

TEST test_length_multi() {
  list_t *l = list_create();
  for (int i = 0; i < 100; i++) {
    list_push_back(l, list_node_create(i));
  }
  ASSERT_EQ(100, list_length(l));
  list_free(l);
  PASS();
}

TEST test_length_after_clear() {
  list_t *l = list_create();
  for (int i = 0; i < 5; i++) list_push_back(l, list_node_create(i));
  for (int i = 0; i < 5; i++) list_pop_front(l);
  ASSERT_EQ(0, list_length(l));
  list_free(l);
  PASS();
}

TEST test_length_after_removes() {
  list_t *l = list_create();
  list_node_t *n1 = list_node_create(1);
  list_node_t *n2 = list_node_create(2);
  list_push_back(l, n1);
  list_push_back(l, n2);
  list_remove(l, n1);
  ASSERT_EQ(1, list_length(l));
  list_free(l);
  PASS();
}

SUITE(suite_list_length) {
  RUN_TEST(test_length_empty);
  RUN_TEST(test_length_single);
  RUN_TEST(test_length_multi);
  RUN_TEST(test_length_after_clear);
  RUN_TEST(test_length_after_removes);
}

TEST test_empty_true() {
  list_t *l = list_create();
  ASSERT(list_empty(l));
  list_free(l);
  PASS();
}

TEST test_empty_false() {
  list_t *l = list_create();
  list_push_back(l, list_node_create(1));
  ASSERT(!list_empty(l));
  list_free(l);
  PASS();
}

TEST test_empty_after_pop_all() {
  list_t *l = list_create();
  list_push_back(l, list_node_create(1));
  list_pop_back(l);
  ASSERT(list_empty(l));
  list_free(l);
  PASS();
}

TEST test_empty_after_remove_only() {
  list_t *l = list_create();
  list_node_t *n = list_node_create(1);
  list_push_back(l, n);
  list_remove(l, n);
  ASSERT(list_empty(l));
  list_free(l);
  PASS();
}

SUITE(suite_list_empty) {
  RUN_TEST(test_empty_true);
  RUN_TEST(test_empty_false);
  RUN_TEST(test_empty_after_pop_all);
  RUN_TEST(test_empty_after_remove_only);
}

TEST test_insert_after_middle() {
  list_t *l = list_create();
  list_node_t *n1 = list_node_create(1);
  list_node_t *n2 = list_node_create(2);
  list_node_t *n3 = list_node_create(3);
  list_push_back(l, n1);
  list_push_back(l, n3);
  list_insert_after(l, n1, n2);
  ASSERT_EQ(n2, n1->next);
  ASSERT_EQ(n2, n3->prev);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

TEST test_insert_after_tail() {
  list_t *l = list_create();
  list_node_t *n1 = list_node_create(1);
  list_node_t *n2 = list_node_create(2);
  list_push_back(l, n1);
  list_insert_after(l, n1, n2);
  ASSERT_EQ(l->tail, n2);
  ASSERT_EQ(n1->next, n2);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

TEST test_insert_after_head() {
  list_t *l = list_create();
  list_node_t *h = list_node_create(1);
  list_push_back(l, h);
  list_insert_after(l, h, list_node_create(2));
  ASSERT_EQ(2, l->head->next->value);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

TEST test_insert_after_sequential() {
  list_t *l = list_create();
  list_node_t *curr = list_node_create(0);
  list_push_back(l, curr);
  for (int i = 1; i < 5; i++) {
    list_node_t *n = list_node_create(i);
    list_insert_after(l, curr, n);
    curr = n;
  }
  ASSERT_EQ(4, l->tail->value);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

SUITE(suite_list_insert_after) {
  RUN_TEST(test_insert_after_middle);
  RUN_TEST(test_insert_after_tail);
  RUN_TEST(test_insert_after_head);
  RUN_TEST(test_insert_after_sequential);
}

TEST test_insert_before_middle() {
  list_t *l = list_create();
  list_node_t *n1 = list_node_create(1);
  list_node_t *n2 = list_node_create(2);
  list_node_t *n3 = list_node_create(3);
  list_push_back(l, n1);
  list_push_back(l, n3);
  list_insert_before(l, n3, n2);
  ASSERT_EQ(n2, n3->prev);
  ASSERT_EQ(n2, n1->next);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

TEST test_insert_before_head() {
  list_t *l = list_create();
  list_node_t *n1 = list_node_create(1);
  list_node_t *n2 = list_node_create(2);
  list_push_back(l, n2);
  list_insert_before(l, n2, n1);
  ASSERT_EQ(l->head, n1);
  ASSERT_EQ(n2->prev, n1);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

TEST test_insert_before_tail() {
  list_t *l = list_create();
  list_node_t *t = list_node_create(10);
  list_push_back(l, t);
  list_insert_before(l, t, list_node_create(5));
  ASSERT_EQ(5, l->tail->prev->value);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

TEST test_insert_before_all() {
  list_t *l = list_create();
  list_node_t *t = list_node_create(100);
  list_push_back(l, t);
  for (int i = 0; i < 5; i++) {
    list_insert_before(l, t, list_node_create(i));
  }
  ASSERT_EQ(0, l->head->value);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

SUITE(suite_list_insert_before) {
  RUN_TEST(test_insert_before_middle);
  RUN_TEST(test_insert_before_head);
  RUN_TEST(test_insert_before_tail);
  RUN_TEST(test_insert_before_all);
}

TEST test_push_front_empty() {
  list_t *l = list_create();
  list_node_t *n = list_node_create(1);
  list_push_front(l, n);
  ASSERT_EQ(l->head, n);
  ASSERT_EQ(l->tail, n);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

TEST test_push_front_multi() {
  list_t *l = list_create();
  list_push_front(l, list_node_create(2));
  list_push_front(l, list_node_create(1));
  ASSERT_EQ(1, l->head->value);
  ASSERT_EQ(2, l->tail->value);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

TEST test_push_front_large_range() {
  list_t *l = list_create();
  for (int i = 0; i < 50; i++) list_push_front(l, list_node_create(i));
  ASSERT_EQ(49, l->head->value);
  ASSERT_EQ(0, l->tail->value);
  list_free(l);
  PASS();
}

TEST test_push_front_tail_check() {
  list_t *l = list_create();
  list_push_front(l, list_node_create(100));
  list_push_front(l, list_node_create(200));
  ASSERT_EQ(100, l->tail->value);
  list_free(l);
  PASS();
}

SUITE(suite_list_push_front) {
  RUN_TEST(test_push_front_empty);
  RUN_TEST(test_push_front_multi);
  RUN_TEST(test_push_front_large_range);
  RUN_TEST(test_push_front_tail_check);
}

TEST test_push_back_empty() {
  list_t *l = list_create();
  list_node_t *n = list_node_create(1);
  list_push_back(l, n);
  ASSERT_EQ(l->head, n);
  ASSERT_EQ(l->tail, n);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

TEST test_push_back_multi() {
  list_t *l = list_create();
  list_push_back(l, list_node_create(1));
  list_push_back(l, list_node_create(2));
  ASSERT_EQ(1, l->head->value);
  ASSERT_EQ(2, l->tail->value);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

TEST test_push_back_large_range() {
  list_t *l = list_create();
  for (int i = 0; i < 50; i++) list_push_back(l, list_node_create(i));
  ASSERT_EQ(0, l->head->value);
  ASSERT_EQ(49, l->tail->value);
  list_free(l);
  PASS();
}

TEST test_push_back_head_check() {
  list_t *l = list_create();
  list_push_back(l, list_node_create(55));
  list_push_back(l, list_node_create(66));
  ASSERT_EQ(55, l->head->value);
  list_free(l);
  PASS();
}

SUITE(suite_list_push_back) {
  RUN_TEST(test_push_back_empty);
  RUN_TEST(test_push_back_multi);
  RUN_TEST(test_push_back_large_range);
  RUN_TEST(test_push_back_head_check);
}

TEST test_front_accessor() {
  list_t *l = list_create();
  list_node_t *n = list_node_create(42);
  list_push_back(l, n);
  ASSERT_EQ(n, list_front(l));
  list_push_front(l, list_node_create(7));
  ASSERT_EQ(7, list_front(l)->value);
  list_free(l);
  PASS();
}

TEST test_front_after_pop_back() {
  list_t *l = list_create();
  list_push_back(l, list_node_create(10));
  list_push_back(l, list_node_create(20));
  list_pop_back(l);
  ASSERT_EQ(10, list_front(l)->value);
  list_free(l);
  PASS();
}

TEST test_front_on_reversed() {
  list_t *l = list_create();
  list_push_back(l, list_node_create(1));
  list_push_back(l, list_node_create(2));
  list_reverse(l);
  ASSERT_EQ(2, list_front(l)->value);
  list_free(l);
  PASS();
}

SUITE(suite_list_front) {
  RUN_TEST(test_front_accessor);
  RUN_TEST(test_front_after_pop_back);
  RUN_TEST(test_front_on_reversed);
}

TEST test_back_accessor() {
  list_t *l = list_create();
  list_node_t *n = list_node_create(42);
  list_push_back(l, n);
  ASSERT_EQ(n, list_back(l));
  list_push_back(l, list_node_create(99));
  ASSERT_EQ(99, list_back(l)->value);
  list_free(l);
  PASS();
}

TEST test_back_after_pop_front() {
  list_t *l = list_create();
  list_push_back(l, list_node_create(1));
  list_push_back(l, list_node_create(2));
  list_pop_front(l);
  ASSERT_EQ(2, list_back(l)->value);
  list_free(l);
  PASS();
}

TEST test_back_on_reversed() {
  list_t *l = list_create();
  list_push_back(l, list_node_create(1));
  list_push_back(l, list_node_create(2));
  list_reverse(l);
  ASSERT_EQ(1, list_back(l)->value);
  list_free(l);
  PASS();
}

SUITE(suite_list_back) {
  RUN_TEST(test_back_accessor);
  RUN_TEST(test_back_after_pop_front);
  RUN_TEST(test_back_on_reversed);
}

TEST test_pop_front_single() {
  list_t *l = list_create();
  list_push_back(l, list_node_create(1));
  list_pop_front(l);
  ASSERT(list_empty(l));
  ASSERT_EQ(NULL, l->head);
  ASSERT_EQ(NULL, l->tail);
  list_free(l);
  PASS();
}

TEST test_pop_front_multi() {
  list_t *l = list_create();
  list_push_back(l, list_node_create(1));
  list_push_back(l, list_node_create(2));
  list_pop_front(l);
  ASSERT_EQ(2, l->head->value);
  ASSERT_EQ(NULL, l->head->prev);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

TEST test_pop_front_two_nodes() {
  list_t *l = list_create();
  list_push_back(l, list_node_create(100));
  list_push_back(l, list_node_create(200));
  list_pop_front(l);
  ASSERT_EQ(l->head, l->tail);
  ASSERT_EQ(200, l->head->value);
  list_free(l);
  PASS();
}

TEST test_pop_front_until_empty() {
  list_t *l = list_create();
  for (int i = 0; i < 3; i++) list_push_back(l, list_node_create(i));
  while (!list_empty(l)) list_pop_front(l);
  ASSERT_EQ(NULL, l->head);
  list_free(l);
  PASS();
}

SUITE(suite_list_pop_front) {
  RUN_TEST(test_pop_front_single);
  RUN_TEST(test_pop_front_multi);
  RUN_TEST(test_pop_front_two_nodes);
  RUN_TEST(test_pop_front_until_empty);
}

TEST test_pop_back_single() {
  list_t *l = list_create();
  list_push_back(l, list_node_create(1));
  list_pop_back(l);
  ASSERT(list_empty(l));
  list_free(l);
  PASS();
}

TEST test_pop_back_multi() {
  list_t *l = list_create();
  list_push_back(l, list_node_create(1));
  list_push_back(l, list_node_create(2));
  list_pop_back(l);
  ASSERT_EQ(1, l->tail->value);
  ASSERT_EQ(NULL, l->tail->next);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

TEST test_pop_back_two_nodes() {
  list_t *l = list_create();
  list_push_back(l, list_node_create(5));
  list_push_back(l, list_node_create(10));
  list_pop_back(l);
  ASSERT_EQ(l->head, l->tail);
  ASSERT_EQ(5, l->tail->value);
  list_free(l);
  PASS();
}

TEST test_pop_back_until_empty() {
  list_t *l = list_create();
  for (int i = 0; i < 3; i++) list_push_back(l, list_node_create(i));
  while (!list_empty(l)) list_pop_back(l);
  ASSERT_EQ(NULL, l->tail);
  list_free(l);
  PASS();
}

SUITE(suite_list_pop_back) {
  RUN_TEST(test_pop_back_single);
  RUN_TEST(test_pop_back_multi);
  RUN_TEST(test_pop_back_two_nodes);
  RUN_TEST(test_pop_back_until_empty);
}

TEST test_remove_head() {
  list_t *l = list_create();
  list_node_t *n1 = list_node_create(1);
  list_node_t *n2 = list_node_create(2);
  list_push_back(l, n1);
  list_push_back(l, n2);
  list_remove(l, n1);
  ASSERT_EQ(l->head, n2);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

TEST test_remove_tail() {
  list_t *l = list_create();
  list_node_t *n1 = list_node_create(1);
  list_node_t *n2 = list_node_create(2);
  list_push_back(l, n1);
  list_push_back(l, n2);
  list_remove(l, n2);
  ASSERT_EQ(l->tail, n1);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

TEST test_remove_head_to_empty() {
  list_t *l = list_create();
  list_node_t *n = list_node_create(55);
  list_push_back(l, n);
  list_remove(l, n);
  ASSERT(list_empty(l));
  list_free(l);
  PASS();
}

TEST test_remove_middle_to_end() {
  list_t *l = list_create();
  list_node_t *n1 = list_node_create(1);
  list_node_t *n2 = list_node_create(2);
  list_node_t *n3 = list_node_create(3);
  list_push_back(l, n1);
  list_push_back(l, n2);
  list_push_back(l, n3);
  list_remove(l, n2);
  ASSERT_EQ(n1->next, n3);
  list_remove(l, n3);
  ASSERT_EQ(l->tail, n1);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

SUITE(suite_list_remove) {
  RUN_TEST(test_remove_head);
  RUN_TEST(test_remove_tail);
  RUN_TEST(test_remove_head_to_empty);
  RUN_TEST(test_remove_middle_to_end);
}

TEST test_merge_standard() {
  list_t *d = list_create();
  list_t *s = list_create();
  list_push_back(d, list_node_create(1));
  list_push_back(d, list_node_create(3));
  list_push_back(s, list_node_create(2));
  list_push_back(s, list_node_create(4));
  list_merge(d, s);
  ASSERT_EQ(4, list_length(d));
  ASSERT(list_empty(s));
  list_node_t *curr = d->head;
  for (int i = 1; i <= 4; i++) {
    ASSERT_EQ(i, curr->value);
    curr = curr->next;
  }
  list_sanity_check(d);
  list_free(d);
  list_free(s);
  PASS();
}

TEST test_merge_into_empty() {
  list_t *d = list_create();
  list_t *s = list_create();
  list_push_back(s, list_node_create(1));
  list_merge(d, s);
  ASSERT_EQ(1, list_length(d));
  ASSERT(list_empty(s));
  list_sanity_check(d);
  list_free(d);
  list_free(s);
  PASS();
}

TEST test_merge_negative_values() {
  list_t *d = list_create();
  list_t *s = list_create();
  list_push_back(d, list_node_create(-10));
  list_push_back(d, list_node_create(0));
  list_push_back(s, list_node_create(-5));
  list_push_back(s, list_node_create(5));
  list_merge(d, s);
  ASSERT_EQ(-10, d->head->value);
  ASSERT_EQ(5, d->tail->value);
  list_sanity_check(d);
  list_free(d);
  list_free(s);
  PASS();
}

TEST test_merge_identical() {
  list_t *d = list_create();
  list_t *s = list_create();
  list_push_back(d, list_node_create(1));
  list_push_back(s, list_node_create(1));
  list_merge(d, s);
  ASSERT_EQ(1, d->head->value);
  ASSERT_EQ(1, d->head->next->value);
  list_free(d);
  list_free(s);
  PASS();
}

SUITE(suite_list_merge) {
  RUN_TEST(test_merge_standard);
  RUN_TEST(test_merge_into_empty);
  RUN_TEST(test_merge_negative_values);
  RUN_TEST(test_merge_identical);
}

TEST test_sort_already_sorted() {
  list_t *l = list_create();
  for (int i = 0; i < 5; i++) list_push_back(l, list_node_create(i));
  list_sort(l);
  ASSERT_EQ(0, l->head->value);
  ASSERT_EQ(4, l->tail->value);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

TEST test_sort_reversed() {
  list_t *l = list_create();
  for (int i = 5; i > 0; i--) list_push_back(l, list_node_create(i));
  list_sort(l);
  ASSERT_EQ(1, l->head->value);
  ASSERT_EQ(5, l->tail->value);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

TEST test_sort_single_element() {
  list_t *l = list_create();
  list_push_back(l, list_node_create(10));
  list_sort(l);
  ASSERT_EQ(10, l->head->value);
  list_free(l);
  PASS();
}

TEST test_sort_negative_range() {
  list_t *l = list_create();
  list_push_back(l, list_node_create(0));
  list_push_back(l, list_node_create(-5));
  list_push_back(l, list_node_create(5));
  list_sort(l);
  ASSERT_EQ(-5, l->head->value);
  ASSERT_EQ(5, l->tail->value);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

SUITE(suite_list_sort) {
  RUN_TEST(test_sort_already_sorted);
  RUN_TEST(test_sort_reversed);
  RUN_TEST(test_sort_single_element);
  RUN_TEST(test_sort_negative_range);
}

TEST test_reverse_empty_and_single() {
  list_t *l = list_create();
  list_reverse(l);
  ASSERT(list_empty(l));
  list_push_back(l, list_node_create(1));
  list_reverse(l);
  ASSERT_EQ(1, l->head->value);
  ASSERT_EQ(1, l->tail->value);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

TEST test_reverse_even_odd() {
  list_t *l = list_create();
  list_push_back(l, list_node_create(1));
  list_push_back(l, list_node_create(2));
  list_reverse(l);
  ASSERT_EQ(2, l->head->value);
  ASSERT_EQ(1, l->tail->value);
  list_push_back(l, list_node_create(3));
  list_reverse(l);
  ASSERT_EQ(3, l->head->value);
  ASSERT_EQ(2, l->tail->value);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

TEST test_reverse_twice() {
  list_t *l = list_create();
  list_push_back(l, list_node_create(1));
  list_push_back(l, list_node_create(2));
  list_reverse(l);
  list_reverse(l);
  ASSERT_EQ(1, l->head->value);
  ASSERT_EQ(2, l->tail->value);
  list_free(l);
  PASS();
}

TEST test_reverse_palindrome() {
  list_t *l = list_create();
  list_push_back(l, list_node_create(1));
  list_push_back(l, list_node_create(2));
  list_push_back(l, list_node_create(1));
  list_reverse(l);
  ASSERT_EQ(1, l->head->value);
  ASSERT_EQ(1, l->tail->value);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

SUITE(suite_list_reverse) {
  RUN_TEST(test_reverse_empty_and_single);
  RUN_TEST(test_reverse_even_odd);
  RUN_TEST(test_reverse_twice);
  RUN_TEST(test_reverse_palindrome);
}

TEST test_unique_all_same() {
  list_t *l = list_create();
  for (int i = 0; i < 5; i++) list_push_back(l, list_node_create(42));
  list_unique(l);
  ASSERT_EQ(1, list_length(l));
  ASSERT_EQ(42, l->head->value);
  ASSERT_EQ(NULL, l->head->next);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

TEST test_unique_alternating() {
  list_t *l = list_create();
  list_push_back(l, list_node_create(1));
  list_push_back(l, list_node_create(1));
  list_push_back(l, list_node_create(2));
  list_push_back(l, list_node_create(1));
  list_unique(l);
  ASSERT_EQ(3, list_length(l));
  list_sanity_check(l);
  list_free(l);
  PASS();
}

TEST test_unique_empty_list() {
  list_t *l = list_create();
  list_unique(l);
  ASSERT(list_empty(l));
  list_free(l);
  PASS();
}

TEST test_unique_end_of_list() {
  list_t *l = list_create();
  list_push_back(l, list_node_create(1));
  list_push_back(l, list_node_create(2));
  list_push_back(l, list_node_create(2));
  list_unique(l);
  ASSERT_EQ(2, l->tail->value);
  ASSERT_EQ(2, list_length(l));
  list_sanity_check(l);
  list_free(l);
  PASS();
}

SUITE(suite_list_unique) {
  RUN_TEST(test_unique_all_same);
  RUN_TEST(test_unique_alternating);
  RUN_TEST(test_unique_empty_list);
  RUN_TEST(test_unique_end_of_list);
}

TEST test_stress_large_list() {
  list_t *l = list_create();
  for (int i = 0; i < 5000; i++) {
    list_push_back(l, list_node_create(i));
  }
  ASSERT_EQ(5000, list_length(l));
  list_reverse(l);
  ASSERT_EQ(4999, l->head->value);
  ASSERT_EQ(0, l->tail->value);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

TEST test_stress_sort_large() {
  list_t *l = list_create();
  for (int i = 500; i >= 0; i--) {
    list_push_back(l, list_node_create(i));
  }
  list_sort(l);
  ASSERT_EQ(0, l->head->value);
  ASSERT_EQ(500, l->tail->value);
  list_sanity_check(l);
  list_free(l);
  PASS();
}

TEST test_stress_push_front_pop_back() {
  list_t *l = list_create();
  for (int i = 0; i < 2000; i++) list_push_front(l, list_node_create(i));
  for (int i = 0; i < 1000; i++) list_pop_back(l);
  ASSERT_EQ(1000, list_length(l));
  list_sanity_check(l);
  list_free(l);
  PASS();
}

TEST test_stress_alternating_ops() {
  list_t *l = list_create();
  for (int i = 0; i < 500; i++) {
    list_push_back(l, list_node_create(i));
    list_push_front(l, list_node_create(i));
  }
  ASSERT_EQ(1000, list_length(l));
  list_sort(l);
  list_unique(l);
  ASSERT_EQ(500, list_length(l));
  list_sanity_check(l);
  list_free(l);
  PASS();
}

SUITE(suite_stress_tests) {
  RUN_TEST(test_stress_large_list);
  RUN_TEST(test_stress_sort_large);
  RUN_TEST(test_stress_push_front_pop_back);
  RUN_TEST(test_stress_alternating_ops);
}

TEST test_scenario_create_sort_unique() {
  list_t *l = list_create();
  int vals[] = {10, 5, 10, 2, 5, 2, 8};
  for (int i = 0; i < 7; i++) list_push_back(l, list_node_create(vals[i]));
  list_sort(l);
  list_unique(l);
  ASSERT_EQ(4, list_length(l));
  int expected[] = {2, 5, 8, 10};
  list_node_t *curr = l->head;
  for (int i = 0; i < 4; i++) {
    ASSERT_EQ(expected[i], curr->value);
    curr = curr->next;
  }
  list_sanity_check(l);
  list_free(l);
  PASS();
}

TEST test_scenario_reverse_merge() {
  list_t *l1 = list_create();
  list_t *l2 = list_create();
  for (int i = 0; i < 3; i++) list_push_back(l1, list_node_create(i));
  for (int i = 3; i < 6; i++) list_push_back(l2, list_node_create(i));
  list_reverse(l1);
  list_reverse(l2);
  list_sort(l1);
  list_sort(l2);
  list_merge(l1, l2);
  ASSERT_EQ(6, list_length(l1));
  list_sanity_check(l1);
  list_free(l1);
  list_free(l2);
  PASS();
}

TEST test_scenario_pop_push_mix() {
  list_t *l = list_create();
  list_push_back(l, list_node_create(1));
  list_pop_front(l);
  list_push_front(l, list_node_create(2));
  list_pop_back(l);
  ASSERT(list_empty(l));
  list_push_back(l, list_node_create(3));
  ASSERT_EQ(3, l->head->value);
  list_free(l);
  PASS();
}

TEST test_scenario_complex_reordering() {
  list_t *l = list_create();
  for (int i = 0; i < 10; i++) list_push_back(l, list_node_create(i % 3));
  list_sort(l);
  list_unique(l);
  list_reverse(l);
  ASSERT_EQ(2, l->head->value);
  ASSERT_EQ(0, l->tail->value);
  ASSERT_EQ(3, list_length(l));
  list_sanity_check(l);
  list_free(l);
  PASS();
}

SUITE(suite_scenario_tests) {
  RUN_TEST(test_scenario_create_sort_unique);
  RUN_TEST(test_scenario_reverse_merge);
  RUN_TEST(test_scenario_pop_push_mix);
  RUN_TEST(test_scenario_complex_reordering);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(suite_list_length);
  RUN_SUITE(suite_list_empty);
  RUN_SUITE(suite_list_insert_after);
  RUN_SUITE(suite_list_insert_before);
  RUN_SUITE(suite_list_push_front);
  RUN_SUITE(suite_list_push_back);
  RUN_SUITE(suite_list_front);
  RUN_SUITE(suite_list_back);
  RUN_SUITE(suite_list_pop_front);
  RUN_SUITE(suite_list_pop_back);
  RUN_SUITE(suite_list_remove);
  RUN_SUITE(suite_list_merge);
  RUN_SUITE(suite_list_sort);
  RUN_SUITE(suite_list_reverse);
  RUN_SUITE(suite_list_unique);
  RUN_SUITE(suite_stress_tests);
  RUN_SUITE(suite_scenario_tests);
  GREATEST_PRINT_REPORT();
  custom_tests();
  return greatest_all_passed() ? EXIT_SUCCESS : EXIT_FAILURE;
}
