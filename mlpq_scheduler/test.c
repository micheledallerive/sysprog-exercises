#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "custom_tests.h"
#include "greatest.h"
#include "lib.h"

static job_t *create_job(job_id_t id, uint8_t priority) {
  job_t *j = malloc(sizeof(job_t));
  j->id = id;
  j->priority = priority;
  j->next = NULL;
  j->prev = NULL;
  return j;
}

TEST init_resets_everything(void) {
  scheduler_t s;
  scheduler_init(&s);
  ASSERT_EQ(0ULL, s.ready_bitset);
  for (int i = 0; i < MAX_PRIORITY; i++) {
    ASSERT_EQ(NULL, s.queues[i].head);
    ASSERT_EQ(NULL, s.queues[i].tail);
  }
  PASS();
}

TEST add_single_job_retrieves_correctly(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *j = create_job(1, 10);
  scheduler_add_job(&s, j);
  ASSERT_EQ(1ULL << 10, s.ready_bitset);
  job_t *out = scheduler_next(&s);
  ASSERT_EQ(j, out);
  ASSERT_EQ(0ULL, s.ready_bitset);
  free(j);
  PASS();
}

TEST add_multiple_different_priorities_retrieves_highest(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *j1 = create_job(1, 10);
  job_t *j2 = create_job(2, 20);
  scheduler_add_job(&s, j1);
  scheduler_add_job(&s, j2);
  job_t *out = scheduler_next(&s);
  ASSERT_EQ(j2, out);
  free(j1);
  free(j2);
  PASS();
}

TEST add_multiple_same_priority_fifo_order(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *j1 = create_job(1, 5);
  job_t *j2 = create_job(2, 5);
  scheduler_add_job(&s, j1);
  scheduler_add_job(&s, j2);
  ASSERT_EQ(j1, scheduler_next(&s));
  ASSERT_EQ(j2, scheduler_next(&s));
  free(j1);
  free(j2);
  PASS();
}

TEST next_returns_null_on_empty(void) {
  scheduler_t s;
  scheduler_init(&s);
  ASSERT_EQ(NULL, scheduler_next(&s));
  PASS();
}

TEST remove_only_job_clears_bitmask(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *j = create_job(1, 15);
  scheduler_add_job(&s, j);
  scheduler_remove_job(&s, j);
  ASSERT_EQ(0ULL, s.ready_bitset);
  ASSERT_EQ(NULL, scheduler_next(&s));
  free(j);
  PASS();
}

TEST remove_head_of_queue(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *j1 = create_job(1, 5);
  job_t *j2 = create_job(2, 5);
  scheduler_add_job(&s, j1);
  scheduler_add_job(&s, j2);
  scheduler_remove_job(&s, j1);
  ASSERT_EQ(j2, scheduler_next(&s));
  free(j1);
  free(j2);
  PASS();
}

TEST remove_tail_of_queue(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *j1 = create_job(1, 5);
  job_t *j2 = create_job(2, 5);
  scheduler_add_job(&s, j1);
  scheduler_add_job(&s, j2);
  scheduler_remove_job(&s, j2);
  ASSERT_EQ(j1, scheduler_next(&s));
  ASSERT_EQ(NULL, scheduler_next(&s));
  free(j1);
  free(j2);
  PASS();
}

TEST remove_middle_of_queue(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *j1 = create_job(1, 5);
  job_t *j2 = create_job(2, 5);
  job_t *j3 = create_job(3, 5);
  scheduler_add_job(&s, j1);
  scheduler_add_job(&s, j2);
  scheduler_add_job(&s, j3);
  scheduler_remove_job(&s, j2);
  ASSERT_EQ(j1, scheduler_next(&s));
  ASSERT_EQ(j3, scheduler_next(&s));
  free(j1);
  free(j2);
  free(j3);
  PASS();
}

TEST add_job_max_priority(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *j = create_job(1, 63);
  scheduler_add_job(&s, j);
  ASSERT_EQ(1ULL << 63, s.ready_bitset);
  ASSERT_EQ(j, scheduler_next(&s));
  free(j);
  PASS();
}

TEST add_job_min_priority(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *j = create_job(1, 0);
  scheduler_add_job(&s, j);
  ASSERT_EQ(1ULL, s.ready_bitset);
  ASSERT_EQ(j, scheduler_next(&s));
  free(j);
  PASS();
}

TEST sequence_add_low_then_high_priority(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *low = create_job(1, 0);
  job_t *high = create_job(2, 63);
  scheduler_add_job(&s, low);
  scheduler_add_job(&s, high);
  ASSERT_EQ(high, scheduler_next(&s));
  ASSERT_EQ(low, scheduler_next(&s));
  free(low);
  free(high);
  PASS();
}

TEST bitmask_set_all_bits(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *jobs[64];
  for (int i = 0; i < 64; i++) {
    jobs[i] = create_job(i, i);
    scheduler_add_job(&s, jobs[i]);
  }
  ASSERT_EQ(0xFFFFFFFFFFFFFFFFULL, s.ready_bitset);
  for (int i = 63; i >= 0; i--) {
    ASSERT_EQ(jobs[i], scheduler_next(&s));
    free(jobs[i]);
  }
  PASS();
}

TEST re_add_job_after_removal(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *j = create_job(1, 10);
  scheduler_add_job(&s, j);
  scheduler_remove_job(&s, j);
  scheduler_add_job(&s, j);
  ASSERT_EQ(j, scheduler_next(&s));
  free(j);
  PASS();
}

TEST verify_pointers_scrubbed_after_pop(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *j1 = create_job(1, 5);
  job_t *j2 = create_job(2, 5);
  scheduler_add_job(&s, j1);
  scheduler_add_job(&s, j2);
  job_t *out = scheduler_next(&s);
  ASSERT_EQ(NULL, out->next);
  ASSERT_EQ(NULL, out->prev);
  free(j1);
  free(j2);
  PASS();
}

TEST verify_pointers_scrubbed_after_remove(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *j1 = create_job(1, 5);
  job_t *j2 = create_job(2, 5);
  scheduler_add_job(&s, j1);
  scheduler_add_job(&s, j2);
  scheduler_remove_job(&s, j1);
  ASSERT_EQ(NULL, j1->next);
  ASSERT_EQ(NULL, j1->prev);
  free(j1);
  free(j2);
  PASS();
}

TEST multiple_queues_active_simultaneously(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *j1 = create_job(1, 10);
  job_t *j2 = create_job(2, 20);
  job_t *j3 = create_job(3, 10);
  scheduler_add_job(&s, j1);
  scheduler_add_job(&s, j2);
  scheduler_add_job(&s, j3);
  ASSERT_EQ(j2, scheduler_next(&s));
  ASSERT_EQ(j1, scheduler_next(&s));
  ASSERT_EQ(j3, scheduler_next(&s));
  free(j1);
  free(j2);
  free(j3);
  PASS();
}

TEST interleaved_add_next_remove(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *j1 = create_job(1, 30);
  scheduler_add_job(&s, j1);
  job_t *out = scheduler_next(&s);
  ASSERT_EQ(j1, out);
  job_t *j2 = create_job(2, 30);
  scheduler_add_job(&s, j2);
  scheduler_remove_job(&s, j2);
  ASSERT_EQ(NULL, scheduler_next(&s));
  free(j1);
  free(j2);
  PASS();
}

TEST stress_add_large_number_same_priority(void) {
  scheduler_t s;
  scheduler_init(&s);
  const int count = 1000;
  job_t *jobs[count];
  for (int i = 0; i < count; i++) {
    jobs[i] = create_job(i, 32);
    scheduler_add_job(&s, jobs[i]);
  }
  for (int i = 0; i < count; i++) {
    ASSERT_EQ(jobs[i], scheduler_next(&s));
    free(jobs[i]);
  }
  PASS();
}

TEST bitmask_integrity_under_partial_removal(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *j1 = create_job(1, 40);
  job_t *j2 = create_job(2, 40);
  scheduler_add_job(&s, j1);
  scheduler_add_job(&s, j2);
  scheduler_remove_job(&s, j1);
  ASSERT_EQ(1ULL << 40, s.ready_bitset);
  scheduler_remove_job(&s, j2);
  ASSERT_EQ(0ULL, s.ready_bitset);
  free(j1);
  free(j2);
  PASS();
}

TEST recovery_after_full_clear(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *j1 = create_job(1, 10);
  scheduler_add_job(&s, j1);
  scheduler_next(&s);
  job_t *j2 = create_job(2, 10);
  scheduler_add_job(&s, j2);
  ASSERT_EQ(j2, scheduler_next(&s));
  free(j1);
  free(j2);
  PASS();
}

TEST priority_63_boundary_check(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *j = create_job(1, 63);
  scheduler_add_job(&s, j);
  uint8_t p = 63 - __builtin_clzll(s.ready_bitset);
  ASSERT_EQ(63, p);
  ASSERT_EQ(j, scheduler_next(&s));
  free(j);
  PASS();
}

TEST random_priority_ordering(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *j_low = create_job(1, 10);
  job_t *j_mid = create_job(2, 30);
  job_t *j_high = create_job(3, 50);
  scheduler_add_job(&s, j_low);
  scheduler_add_job(&s, j_high);
  scheduler_add_job(&s, j_mid);
  ASSERT_EQ(j_high, scheduler_next(&s));
  ASSERT_EQ(j_mid, scheduler_next(&s));
  ASSERT_EQ(j_low, scheduler_next(&s));
  free(j_low);
  free(j_mid);
  free(j_high);
  PASS();
}

TEST stress_massive_churn(void) {
  scheduler_t s;
  scheduler_init(&s);
  for (int iteration = 0; iteration < 100; iteration++) {
    job_t *j = create_job(iteration, iteration % 64);
    scheduler_add_job(&s, j);
    ASSERT_EQ(j, scheduler_next(&s));
    free(j);
  }
  ASSERT_EQ(0ULL, s.ready_bitset);
  PASS();
}

TEST queue_tail_integrity_check(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *j1 = create_job(1, 2);
  job_t *j2 = create_job(2, 2);
  scheduler_add_job(&s, j1);
  scheduler_add_job(&s, j2);
  ASSERT_EQ(j2, s.queues[2].tail);
  scheduler_remove_job(&s, j2);
  ASSERT_EQ(j1, s.queues[2].tail);
  free(j1);
  free(j2);
  PASS();
}

TEST double_remove_safety(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *j = create_job(1, 5);
  scheduler_add_job(&s, j);
  scheduler_remove_job(&s, j);
  ASSERT_EQ(0ULL, s.ready_bitset);
  free(j);
  PASS();
}

TEST empty_bitmask_on_all_levels(void) {
  scheduler_t s;
  scheduler_init(&s);
  for (int i = 0; i < 64; i++) {
    job_t *j = create_job(i, i);
    scheduler_add_job(&s, j);
    scheduler_remove_job(&s, j);
    free(j);
  }
  ASSERT_EQ(0ULL, s.ready_bitset);
  PASS();
}

TEST priority_jump_preemption(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *j1 = create_job(1, 10);
  scheduler_add_job(&s, j1);
  job_t *j2 = create_job(2, 60);
  scheduler_add_job(&s, j2);
  ASSERT_EQ(j2, scheduler_next(&s));
  ASSERT_EQ(j1, scheduler_next(&s));
  free(j1);
  free(j2);
  PASS();
}

TEST next_after_all_removed_is_null(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *j1 = create_job(1, 5);
  job_t *j2 = create_job(2, 10);
  scheduler_add_job(&s, j1);
  scheduler_add_job(&s, j2);
  scheduler_remove_job(&s, j1);
  scheduler_remove_job(&s, j2);
  ASSERT_EQ(NULL, scheduler_next(&s));
  free(j1);
  free(j2);
  PASS();
}

TEST large_scale_interleaved_ops(void) {
  scheduler_t s;
  scheduler_init(&s);
  job_t *jobs[100];
  for (int i = 0; i < 100; i++) {
    jobs[i] = create_job(i, i % 64);
    scheduler_add_job(&s, jobs[i]);
  }
  for (int i = 0; i < 50; i++) {
    job_t *out = scheduler_next(&s);
    ASSERT(out != NULL);
  }
  for (int i = 0; i < 100; i++) {
    if (jobs[i]->prev != NULL || jobs[i]->next != NULL) {
      scheduler_remove_job(&s, jobs[i]);
    }
    free(jobs[i]);
  }
  PASS();
}

SUITE(mlpq_suite) {
  RUN_TEST(init_resets_everything);
  RUN_TEST(add_single_job_retrieves_correctly);
  RUN_TEST(add_multiple_different_priorities_retrieves_highest);
  RUN_TEST(add_multiple_same_priority_fifo_order);
  RUN_TEST(next_returns_null_on_empty);
  RUN_TEST(remove_only_job_clears_bitmask);
  RUN_TEST(remove_head_of_queue);
  RUN_TEST(remove_tail_of_queue);
  RUN_TEST(remove_middle_of_queue);
  RUN_TEST(add_job_max_priority);
  RUN_TEST(add_job_min_priority);
  RUN_TEST(sequence_add_low_then_high_priority);
  RUN_TEST(bitmask_set_all_bits);
  RUN_TEST(re_add_job_after_removal);
  RUN_TEST(verify_pointers_scrubbed_after_pop);
  RUN_TEST(verify_pointers_scrubbed_after_remove);
  RUN_TEST(multiple_queues_active_simultaneously);
  RUN_TEST(interleaved_add_next_remove);
  RUN_TEST(stress_add_large_number_same_priority);
  RUN_TEST(bitmask_integrity_under_partial_removal);
  RUN_TEST(recovery_after_full_clear);
  RUN_TEST(priority_63_boundary_check);
  RUN_TEST(random_priority_ordering);
  RUN_TEST(stress_massive_churn);
  RUN_TEST(queue_tail_integrity_check);
  RUN_TEST(double_remove_safety);
  RUN_TEST(empty_bitmask_on_all_levels);
  RUN_TEST(priority_jump_preemption);
  RUN_TEST(next_after_all_removed_is_null);
  RUN_TEST(large_scale_interleaved_ops);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  srand(42);
  GREATEST_MAIN_BEGIN();

  RUN_SUITE(mlpq_suite);

  GREATEST_PRINT_REPORT();
  custom_tests();
  return greatest_all_passed() ? EXIT_SUCCESS : EXIT_FAILURE;
}
