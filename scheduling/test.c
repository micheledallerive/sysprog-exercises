#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "custom_tests.h"
#include "greatest.h"
#include "lib.h"

static int validate_schedule(size_t n, size_t m, const job_dependency_t *deps,
                             job_id_t *res) {
  if (n == 0) return 1;
  if (res == NULL) return 0;

  uint8_t *seen = calloc(n, sizeof(uint8_t));
  for (size_t i = 0; i < n; i++) {
    if (res[i] >= n || seen[res[i]]) {
      free(seen);
      return 0;
    }
    seen[res[i]] = 1;
  }
  free(seen);

  for (size_t i = 0; i < m; i++) {
    int pos_job = -1;
    int pos_dep = -1;
    for (size_t j = 0; j < n; j++) {
      if (res[j] == deps[i].job_id) pos_job = (int)j;
      if (res[j] == deps[i].depends_on_id) pos_dep = (int)j;
    }
    if (pos_job == -1 || pos_dep == -1 || pos_job <= pos_dep) return 0;
  }
  return 1;
}

TEST test_empty_input() {
  job_id_t *res = schedule_jobs(0, 0, NULL);
  PASS();
}

TEST test_single_job() {
  job_id_t *res = schedule_jobs(1, 0, NULL);
  ASSERT(validate_schedule(1, 0, NULL, res));
  free(res);
  PASS();
}

TEST test_two_jobs_no_deps() {
  job_id_t *res = schedule_jobs(2, 0, NULL);
  ASSERT(validate_schedule(2, 0, NULL, res));
  free(res);
  PASS();
}

TEST test_two_jobs_one_dep() {
  job_dependency_t deps[] = {{1, 0}};
  job_id_t *res = schedule_jobs(2, 1, deps);
  ASSERT(validate_schedule(2, 1, deps, res));
  free(res);
  PASS();
}

TEST test_linear_chain() {
  job_dependency_t deps[] = {{1, 0}, {2, 1}, {3, 2}};
  job_id_t *res = schedule_jobs(4, 3, deps);
  ASSERT(validate_schedule(4, 3, deps, res));
  free(res);
  PASS();
}

TEST test_star_dependency() {
  job_dependency_t deps[] = {{1, 0}, {2, 0}, {3, 0}};
  job_id_t *res = schedule_jobs(4, 3, deps);
  ASSERT(validate_schedule(4, 3, deps, res));
  free(res);
  PASS();
}

TEST test_inverse_star() {
  job_dependency_t deps[] = {{0, 1}, {0, 2}, {0, 3}};
  job_id_t *res = schedule_jobs(4, 3, deps);
  ASSERT(validate_schedule(4, 3, deps, res));
  free(res);
  PASS();
}

TEST test_disconnected_components() {
  job_dependency_t deps[] = {{1, 0}, {3, 2}};
  job_id_t *res = schedule_jobs(4, 2, deps);
  ASSERT(validate_schedule(4, 2, deps, res));
  free(res);
  PASS();
}

TEST test_diamond_dependency() {
  job_dependency_t deps[] = {{1, 0}, {2, 0}, {3, 1}, {3, 2}};
  job_id_t *res = schedule_jobs(4, 4, deps);
  ASSERT(validate_schedule(4, 4, deps, res));
  free(res);
  PASS();
}

TEST test_redundant_dependency() {
  job_dependency_t deps[] = {{1, 0}, {2, 1}, {2, 0}};
  job_id_t *res = schedule_jobs(3, 3, deps);
  ASSERT(validate_schedule(3, 3, deps, res));
  free(res);
  PASS();
}

TEST test_multiple_parents() {
  job_dependency_t deps[] = {{2, 0}, {2, 1}};
  job_id_t *res = schedule_jobs(3, 2, deps);
  ASSERT(validate_schedule(3, 2, deps, res));
  free(res);
  PASS();
}

TEST test_large_linear_chain() {
  size_t n = 100;
  job_dependency_t *deps = malloc((n - 1) * sizeof(job_dependency_t));
  for (size_t i = 0; i < n - 1; i++) {
    deps[i].job_id = (job_id_t)(i + 1);
    deps[i].depends_on_id = (job_id_t)i;
  }
  job_id_t *res = schedule_jobs(n, n - 1, deps);
  ASSERT(validate_schedule(n, n - 1, deps, res));
  free(deps);
  free(res);
  PASS();
}

TEST test_cycle_two_jobs() {
  job_dependency_t deps[] = {{0, 1}, {1, 0}};
  job_id_t *res = schedule_jobs(2, 2, deps);
  ASSERT_EQ(NULL, res);
  PASS();
}

TEST test_self_dependency() {
  job_dependency_t deps[] = {{0, 0}};
  job_id_t *res = schedule_jobs(1, 1, deps);
  ASSERT_EQ(NULL, res);
  PASS();
}

TEST test_complex_dag() {
  job_dependency_t deps[] = {{5, 2}, {5, 0}, {4, 0}, {4, 1}, {2, 3}, {3, 1}};
  job_id_t *res = schedule_jobs(6, 6, deps);
  ASSERT(validate_schedule(6, 6, deps, res));
  free(res);
  PASS();
}

TEST test_all_independent() {
  job_id_t *res = schedule_jobs(10, 0, NULL);
  ASSERT(validate_schedule(10, 0, NULL, res));
  free(res);
  PASS();
}

TEST test_shuffled_ids() {
  job_dependency_t deps[] = {{0, 9}, {9, 5}, {5, 2}};
  job_id_t *res = schedule_jobs(10, 3, deps);
  ASSERT(validate_schedule(10, 3, deps, res));
  free(res);
  PASS();
}

TEST test_dense_graph() {
  job_dependency_t deps[6];
  size_t k = 0;
  for (uint32_t i = 0; i < 4; i++) {
    for (uint32_t j = i + 1; j < 4; j++) {
      deps[k].job_id = j;
      deps[k].depends_on_id = i;
      k++;
    }
  }
  job_id_t *res = schedule_jobs(4, 6, deps);
  ASSERT(validate_schedule(4, 6, deps, res));
  free(res);
  PASS();
}

TEST test_out_of_order_deps() {
  job_dependency_t deps[] = {{3, 2}, {1, 0}, {2, 1}};
  job_id_t *res = schedule_jobs(4, 3, deps);
  ASSERT(validate_schedule(4, 3, deps, res));
  free(res);
  PASS();
}

TEST test_large_independent_set() {
  size_t n = 500;
  job_id_t *res = schedule_jobs(n, 0, NULL);
  ASSERT(validate_schedule(n, 0, NULL, res));
  free(res);
  PASS();
}

SUITE(dependency_suite) {
  RUN_TEST(test_empty_input);
  RUN_TEST(test_single_job);
  RUN_TEST(test_two_jobs_no_deps);
  RUN_TEST(test_two_jobs_one_dep);
  RUN_TEST(test_linear_chain);
  RUN_TEST(test_star_dependency);
  RUN_TEST(test_inverse_star);
  RUN_TEST(test_disconnected_components);
  RUN_TEST(test_diamond_dependency);
  RUN_TEST(test_redundant_dependency);
  RUN_TEST(test_multiple_parents);
  RUN_TEST(test_large_linear_chain);
  RUN_TEST(test_cycle_two_jobs);
  RUN_TEST(test_self_dependency);
  RUN_TEST(test_complex_dag);
  RUN_TEST(test_all_independent);
  RUN_TEST(test_shuffled_ids);
  RUN_TEST(test_dense_graph);
  RUN_TEST(test_out_of_order_deps);
  RUN_TEST(test_large_independent_set);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(dependency_suite);
  GREATEST_PRINT_REPORT();
  custom_tests();
  return greatest_all_passed() ? EXIT_SUCCESS : EXIT_FAILURE;
}
