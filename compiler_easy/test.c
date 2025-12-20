#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "greatest.h"
#include "lib.h"

#define ASSERT_STRUCT_MATCH(fields, type_name)                          \
  do {                                                                  \
    size_t total = compile(sizeof(fields) / sizeof(fields[0]), fields); \
    ASSERT_EQ_FMT(sizeof(type_name), total, "%zu");                     \
  } while (0)

#define ASSERT_FIELD_MATCH(fields, index, type_name, member_name)         \
  do {                                                                    \
    ASSERT_EQ_FMT(offsetof(type_name, member_name), fields[index].offset, \
                  "%zu");                                                 \
  } while (0)

TEST test_empty_struct() {
  size_t total = compile(0, NULL);
  ASSERT_EQ(0, total);
  PASS();
}

TEST test_single_char() {
  struct s {
    char a;
  };
  struct field f[] = {{FIELD_TYPE_CHAR, 0}};
  ASSERT_STRUCT_MATCH(f, struct s);
  ASSERT_FIELD_MATCH(f, 0, struct s, a);
  PASS();
}

TEST test_single_int() {
  struct s {
    int a;
  };
  struct field f[] = {{FIELD_TYPE_INT, 0}};
  ASSERT_STRUCT_MATCH(f, struct s);
  ASSERT_FIELD_MATCH(f, 0, struct s, a);
  PASS();
}

TEST test_char_int() {
  struct s {
    char a;
    int b;
  };
  struct field f[] = {{FIELD_TYPE_CHAR, 0}, {FIELD_TYPE_INT, 0}};
  ASSERT_STRUCT_MATCH(f, struct s);
  ASSERT_FIELD_MATCH(f, 0, struct s, a);
  ASSERT_FIELD_MATCH(f, 1, struct s, b);
  PASS();
}

TEST test_int_char() {
  struct s {
    int a;
    char b;
  };
  struct field f[] = {{FIELD_TYPE_INT, 0}, {FIELD_TYPE_CHAR, 0}};
  ASSERT_STRUCT_MATCH(f, struct s);
  ASSERT_FIELD_MATCH(f, 0, struct s, a);
  ASSERT_FIELD_MATCH(f, 1, struct s, b);
  PASS();
}

TEST test_char_double() {
  struct s {
    char a;
    double b;
  };
  struct field f[] = {{FIELD_TYPE_CHAR, 0}, {FIELD_TYPE_DOUBLE, 0}};
  ASSERT_STRUCT_MATCH(f, struct s);
  ASSERT_FIELD_MATCH(f, 0, struct s, a);
  ASSERT_FIELD_MATCH(f, 1, struct s, b);
  PASS();
}

TEST test_char_char_int() {
  struct s {
    char a;
    char b;
    int c;
  };
  struct field f[] = {
      {FIELD_TYPE_CHAR, 0}, {FIELD_TYPE_CHAR, 0}, {FIELD_TYPE_INT, 0}};
  ASSERT_STRUCT_MATCH(f, struct s);
  ASSERT_FIELD_MATCH(f, 2, struct s, c);
  PASS();
}

TEST test_char_int_char() {
  struct s {
    char a;
    int b;
    char c;
  };
  struct field f[] = {
      {FIELD_TYPE_CHAR, 0}, {FIELD_TYPE_INT, 0}, {FIELD_TYPE_CHAR, 0}};
  ASSERT_STRUCT_MATCH(f, struct s);
  ASSERT_FIELD_MATCH(f, 1, struct s, b);
  ASSERT_FIELD_MATCH(f, 2, struct s, c);
  PASS();
}

TEST test_all_types_ascending() {
  struct s {
    char a;
    int b;
    float c;
    double d;
    void *e;
  };
  struct field f[] = {{FIELD_TYPE_CHAR, 0},
                      {FIELD_TYPE_INT, 0},
                      {FIELD_TYPE_FLOAT, 0},
                      {FIELD_TYPE_DOUBLE, 0},
                      {FIELD_TYPE_POINTER, 0}};
  ASSERT_STRUCT_MATCH(f, struct s);
  ASSERT_FIELD_MATCH(f, 4, struct s, e);
  PASS();
}

TEST test_all_types_descending() {
  struct s {
    void *a;
    double b;
    float c;
    int d;
    char e;
  };
  struct field f[] = {{FIELD_TYPE_POINTER, 0},
                      {FIELD_TYPE_DOUBLE, 0},
                      {FIELD_TYPE_FLOAT, 0},
                      {FIELD_TYPE_INT, 0},
                      {FIELD_TYPE_CHAR, 0}};
  ASSERT_STRUCT_MATCH(f, struct s);
  ASSERT_FIELD_MATCH(f, 4, struct s, e);
  PASS();
}

TEST test_multiple_chars() {
  struct s {
    char a;
    char b;
    char c;
    char d;
    char e;
  };
  struct field f[] = {{FIELD_TYPE_CHAR, 0},
                      {FIELD_TYPE_CHAR, 0},
                      {FIELD_TYPE_CHAR, 0},
                      {FIELD_TYPE_CHAR, 0},
                      {FIELD_TYPE_CHAR, 0}};
  ASSERT_STRUCT_MATCH(f, struct s);
  ASSERT_FIELD_MATCH(f, 4, struct s, e);
  PASS();
}

TEST test_pointer_sandwich() {
  struct s {
    char a;
    void *b;
    char c;
  };
  struct field f[] = {
      {FIELD_TYPE_CHAR, 0}, {FIELD_TYPE_POINTER, 0}, {FIELD_TYPE_CHAR, 0}};
  ASSERT_STRUCT_MATCH(f, struct s);
  ASSERT_FIELD_MATCH(f, 1, struct s, b);
  ASSERT_FIELD_MATCH(f, 2, struct s, c);
  PASS();
}

TEST test_float_int_interleave() {
  struct s {
    float a;
    int b;
    float c;
    int d;
  };
  struct field f[] = {{FIELD_TYPE_FLOAT, 0},
                      {FIELD_TYPE_INT, 0},
                      {FIELD_TYPE_FLOAT, 0},
                      {FIELD_TYPE_INT, 0}};
  ASSERT_STRUCT_MATCH(f, struct s);
  ASSERT_FIELD_MATCH(f, 3, struct s, d);
  PASS();
}

TEST test_large_padding_gap() {
  struct s {
    char a;
    char b;
    double c;
  };
  struct field f[] = {
      {FIELD_TYPE_CHAR, 0}, {FIELD_TYPE_CHAR, 0}, {FIELD_TYPE_DOUBLE, 0}};
  ASSERT_STRUCT_MATCH(f, struct s);
  ASSERT_FIELD_MATCH(f, 2, struct s, c);
  PASS();
}

TEST test_mixed_small_large() {
  struct s {
    char a;
    double b;
    char c;
    int d;
  };
  struct field f[] = {{FIELD_TYPE_CHAR, 0},
                      {FIELD_TYPE_DOUBLE, 0},
                      {FIELD_TYPE_CHAR, 0},
                      {FIELD_TYPE_INT, 0}};
  ASSERT_STRUCT_MATCH(f, struct s);
  ASSERT_FIELD_MATCH(f, 3, struct s, d);
  PASS();
}

TEST test_three_ints() {
  struct s {
    int a;
    int b;
    int c;
  };
  struct field f[] = {
      {FIELD_TYPE_INT, 0}, {FIELD_TYPE_INT, 0}, {FIELD_TYPE_INT, 0}};
  ASSERT_STRUCT_MATCH(f, struct s);
  ASSERT_FIELD_MATCH(f, 2, struct s, c);
  PASS();
}

TEST test_char_pointer_char_pointer() {
  struct s {
    char a;
    void *b;
    char c;
    void *d;
  };
  struct field f[] = {{FIELD_TYPE_CHAR, 0},
                      {FIELD_TYPE_POINTER, 0},
                      {FIELD_TYPE_CHAR, 0},
                      {FIELD_TYPE_POINTER, 0}};
  ASSERT_STRUCT_MATCH(f, struct s);
  ASSERT_FIELD_MATCH(f, 3, struct s, d);
  PASS();
}

TEST test_double_float_char() {
  struct s {
    double a;
    float b;
    char c;
  };
  struct field f[] = {
      {FIELD_TYPE_DOUBLE, 0}, {FIELD_TYPE_FLOAT, 0}, {FIELD_TYPE_CHAR, 0}};
  ASSERT_STRUCT_MATCH(f, struct s);
  ASSERT_FIELD_MATCH(f, 2, struct s, c);
  PASS();
}

TEST test_maximal_alignment_stress() {
  struct s {
    char a;
    double b;
    char c;
    void *d;
    char e;
    int f;
  };
  struct field f_arr[] = {{FIELD_TYPE_CHAR, 0}, {FIELD_TYPE_DOUBLE, 0},
                          {FIELD_TYPE_CHAR, 0}, {FIELD_TYPE_POINTER, 0},
                          {FIELD_TYPE_CHAR, 0}, {FIELD_TYPE_INT, 0}};
  ASSERT_STRUCT_MATCH(f_arr, struct s);
  ASSERT_FIELD_MATCH(f_arr, 5, struct s, f);
  PASS();
}

TEST test_repeated_types() {
  struct s {
    int a;
    int b;
    int c;
    int d;
  };
  struct field f[] = {{FIELD_TYPE_INT, 0},
                      {FIELD_TYPE_INT, 0},
                      {FIELD_TYPE_INT, 0},
                      {FIELD_TYPE_INT, 0}};
  ASSERT_STRUCT_MATCH(f, struct s);
  PASS();
}

SUITE(struct_layout_tests) {
  RUN_TEST(test_empty_struct);
  RUN_TEST(test_single_char);
  RUN_TEST(test_single_int);
  RUN_TEST(test_char_int);
  RUN_TEST(test_int_char);
  RUN_TEST(test_char_double);
  RUN_TEST(test_char_char_int);
  RUN_TEST(test_char_int_char);
  RUN_TEST(test_all_types_ascending);
  RUN_TEST(test_all_types_descending);
  RUN_TEST(test_multiple_chars);
  RUN_TEST(test_pointer_sandwich);
  RUN_TEST(test_float_int_interleave);
  RUN_TEST(test_large_padding_gap);
  RUN_TEST(test_mixed_small_large);
  RUN_TEST(test_three_ints);
  RUN_TEST(test_char_pointer_char_pointer);
  RUN_TEST(test_double_float_char);
  RUN_TEST(test_maximal_alignment_stress);
  RUN_TEST(test_repeated_types);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(struct_layout_tests);
  GREATEST_PRINT_REPORT();
  custom_tests();
  return greatest_all_passed() ? EXIT_SUCCESS : EXIT_FAILURE;
}
