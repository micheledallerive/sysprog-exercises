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

#define INFINITY (__builtin_inff())
#define NAN (__builtin_nanf(""))

TEST test_fp_zero() {
  ASSERT_EQ(FP_ZERO, fpclassify(0.0f));
  PASS();
}

TEST test_fp_neg_zero() {
  ASSERT_EQ(FP_ZERO, fpclassify(-0.0f));
  PASS();
}

TEST test_fp_normal_positive() {
  ASSERT_EQ(FP_NORMAL, fpclassify(1.0f));
  PASS();
}

TEST test_fp_normal_negative() {
  ASSERT_EQ(FP_NORMAL, fpclassify(-1.0f));
  PASS();
}

TEST test_fp_normal_max() {
  ASSERT_EQ(FP_NORMAL, fpclassify(FLT_MAX));
  PASS();
}

TEST test_fp_normal_min() {
  ASSERT_EQ(FP_NORMAL, fpclassify(FLT_MIN));
  PASS();
}

TEST test_fp_subnormal_smallest() {
  ASSERT_EQ(FP_SUBNORMAL, fpclassify(FLT_TRUE_MIN));
  PASS();
}

TEST test_fp_subnormal_mid() {
  ASSERT_EQ(FP_SUBNORMAL, fpclassify(FLT_MIN / 2.0f));
  PASS();
}

TEST test_fp_infinity() {
  ASSERT_EQ(FP_INFINITE, fpclassify(INFINITY));
  PASS();
}

TEST test_fp_neg_infinity() {
  ASSERT_EQ(FP_INFINITE, fpclassify(-INFINITY));
  PASS();
}

TEST test_fp_nan() {
  ASSERT_EQ(FP_NAN, fpclassify(NAN));
  PASS();
}

TEST test_fp_neg_nan() {
  ASSERT_EQ(FP_NAN, fpclassify(-NAN));
  PASS();
}

TEST test_fp_large_normal() {
  ASSERT_EQ(FP_NORMAL, fpclassify(1e30f));
  PASS();
}

TEST test_fp_small_normal() {
  ASSERT_EQ(FP_NORMAL, fpclassify(1.1e-37f));
  PASS();
}

TEST test_fp_random_normal() {
  ASSERT_EQ(FP_NORMAL, fpclassify(42.42f));
  PASS();
}

TEST test_fp_expression_inf() {
  float val = 1.0f / 0.0f;
  ASSERT_EQ(FP_INFINITE, fpclassify(val));
  PASS();
}

TEST test_fp_expression_nan() {
  float val = 0.0f / 0.0f;
  ASSERT_EQ(FP_NAN, fpclassify(val));
  PASS();
}

TEST test_fp_boundary_normal() {
  ASSERT_EQ(FP_NORMAL, fpclassify(FLT_MIN * 1.000001f));
  PASS();
}

TEST test_fp_boundary_subnormal() {
  ASSERT_EQ(FP_SUBNORMAL, fpclassify(FLT_MIN * 0.999999f));
  PASS();
}

TEST test_fp_large_neg_normal() {
  ASSERT_EQ(FP_NORMAL, fpclassify(-1e38f));
  PASS();
}

SUITE(fpclassify_suite) {
  RUN_TEST(test_fp_zero);
  RUN_TEST(test_fp_neg_zero);
  RUN_TEST(test_fp_normal_positive);
  RUN_TEST(test_fp_normal_negative);
  RUN_TEST(test_fp_normal_max);
  RUN_TEST(test_fp_normal_min);
  RUN_TEST(test_fp_subnormal_smallest);
  RUN_TEST(test_fp_subnormal_mid);
  RUN_TEST(test_fp_infinity);
  RUN_TEST(test_fp_neg_infinity);
  RUN_TEST(test_fp_nan);
  RUN_TEST(test_fp_neg_nan);
  RUN_TEST(test_fp_large_normal);
  RUN_TEST(test_fp_small_normal);
  RUN_TEST(test_fp_random_normal);
  RUN_TEST(test_fp_expression_inf);
  RUN_TEST(test_fp_expression_nan);
  RUN_TEST(test_fp_boundary_normal);
  RUN_TEST(test_fp_boundary_subnormal);
  RUN_TEST(test_fp_large_neg_normal);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  srand(42);
  GREATEST_MAIN_BEGIN();

  RUN_SUITE(fpclassify_suite);

  GREATEST_PRINT_REPORT();
  custom_tests();
  return greatest_all_passed() ? EXIT_SUCCESS : EXIT_FAILURE;
}
