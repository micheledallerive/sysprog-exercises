#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "custom_tests.h"
#include "greatest.h"
#include "lib.h"

struct function {
  const char *filename;
  const char *function_name;
  void *ret_addr;
  size_t stack_size;
};

#define STACK_SIZE 512

void *allocate_stack(struct function *funcs, size_t func_count,
                     void **initial_rbp) {
  /**
   * funcs[0] is the bottom function (the first called)
   * funcs[func_count - 1] is the top function (the last called)
   */
  assert(func_count > 0);
  void *stack = malloc(STACK_SIZE);
  if (!stack) {
    return NULL;
  }
  void *current_sp = (char *)stack + STACK_SIZE;
  void *current_rbp = NULL;
  for (int i = 0; i < (int)func_count; ++i) {
    struct function *func = &funcs[i];
    size_t frame_size = func->stack_size;
    current_sp -= 8;
    memcpy(current_sp, &func->ret_addr, 8);  // return address
    current_sp -= 8;
    memcpy(current_sp, &current_rbp, 8);  // previous RBP
    current_rbp = current_sp;
    current_sp -= frame_size;
  }
  if (initial_rbp) {
    *initial_rbp = current_rbp;
  }
  return stack;
}

void funcs_to_dbginfo(size_t num, struct function *funcs,
                      struct debugging_info *dbg_info) {
  for (size_t i = 0; i < num; ++i) {
    dbg_info[i].file_name = funcs[i].filename;
    dbg_info[i].function_name = funcs[i].function_name;
    dbg_info[i].start_addr = (char *)funcs[i].ret_addr - 0x10;
    dbg_info[i].end_addr = (char *)funcs[i].ret_addr + 0x10;
  }
}

void print_stack(void *stack) {
  uint64_t *ptr = (uint64_t *)stack;
  for (size_t i = 0; i < STACK_SIZE / 8; ++i) {
    printf("%02zu: %p\n", i, (void *)ptr[i]);
  }
}

void run_test(char *outbuf, size_t outbuf_sz, struct function *funcs,
              size_t func_count) {
  void *initial_rbp = NULL;
  void *stack = allocate_stack(funcs, func_count, &initial_rbp);
  assert(stack != NULL);

  FILE *output = tmpfile();
  assert(output != NULL);

  struct debugging_info dbg_info[func_count];
  funcs_to_dbginfo(func_count, funcs, dbg_info);

  print_backtrace(output, initial_rbp, dbg_info, func_count);

  fflush(output);
  fseek(output, 0, SEEK_SET);
  size_t total_read = fread(outbuf, 1, outbuf_sz - 1, output);
  outbuf[total_read] = '\0';
}

TEST test1(void) {
  struct function funcs[] = {
      {"file1.c", "main", (void *)0x400100, 32},
      {"file2.c", "funcA", (void *)0x400200, 48},
      {"file3.c", "funcB", (void *)0x400300, 64},
      {"file4.c", "funcC", (void *)0x400400, 80},
  };
  size_t func_count = sizeof(funcs) / sizeof(funcs[0]);

  const char *expected_output =
      "0x400400 file4.c:funcC\n"
      "0x400300 file3.c:funcB\n"
      "0x400200 file2.c:funcA\n"
      "0x400100 file1.c:main\n";

  char buffer[1024];
  run_test(buffer, sizeof(buffer), funcs, func_count);

  ASSERT_STR_EQ(expected_output, buffer);

  PASS();
}

TEST test_one_function(void) {
  struct function funcs[] = {
      {"single.c", "only_function", (void *)0x500100, 32},
  };
  size_t func_count = sizeof(funcs) / sizeof(funcs[0]);

  const char *expected_output = "0x500100 single.c:only_function\n";

  char buffer[1024];
  run_test(buffer, sizeof(buffer), funcs, func_count);

  ASSERT_STR_EQ(expected_output, buffer);

  PASS();
}

TEST test_many_functions(void) {
  const size_t func_count = 20;
  struct function funcs[func_count];
  char expected_output[4096] = "";
  char line[128];

  for (size_t i = 0; i < func_count; ++i) {
    funcs[i].filename = "many.c";
    funcs[i].function_name = "func";
    funcs[i].ret_addr = (void *)(0x600000 + i * 0x100);
    funcs[i].stack_size = 32 + i * 4;

    snprintf(line, sizeof(line), "0x%lx many.c:func\n",
             (uintptr_t)funcs[i].ret_addr);
    strcat(expected_output, line);
  }

  for (size_t i = 0; i < func_count / 2; ++i) {
    struct function temp = funcs[i];
    funcs[i] = funcs[func_count - 1 - i];
    funcs[func_count - 1 - i] = temp;
  }

  char buffer[8192];
  run_test(buffer, sizeof(buffer), funcs, func_count);

  ASSERT_STR_EQ(expected_output, buffer);

  PASS();
}

TEST test_max_stack_utilization(void) {
  struct function funcs[] = {{"file.c", "main", (void *)0x400100, 480}};
  size_t func_count = 1;
  const char *expected = "0x400100 file.c:main\n";
  char buffer[1024];
  run_test(buffer, sizeof(buffer), funcs, func_count);
  ASSERT_STR_EQ(expected, buffer);
  PASS();
}

TEST test_zero_frame_size(void) {
  struct function funcs[] = {{"file.c", "main", (void *)0x400100, 0},
                             {"file.c", "leaf", (void *)0x400200, 0}};
  size_t func_count = 2;
  const char *expected = "0x400200 file.c:leaf\n0x400100 file.c:main\n";
  char buffer[1024];
  run_test(buffer, sizeof(buffer), funcs, func_count);
  ASSERT_STR_EQ(expected, buffer);
  PASS();
}

TEST test_address_at_range_start(void) {
  struct function funcs[] = {{"boundary.c", "start", (void *)0x400000, 32}};
  void *initial_rbp = NULL;
  void *stack = allocate_stack(funcs, 1, &initial_rbp);
  FILE *output = tmpfile();
  struct debugging_info dbg = {
      "boundary.c",
      "start",
      (void *)0x400000,
      (void *)0x400010,
  };

  print_backtrace(output, initial_rbp, &dbg, 1);

  fflush(output);
  fseek(output, 0, SEEK_SET);
  char buffer[1024] = {0};
  fread(buffer, 1, sizeof(buffer) - 1, output);
  fclose(output);
  free(stack);

  ASSERT(strstr(buffer, "start") != NULL);
  PASS();
}

TEST test_address_at_range_end(void) {
  struct function funcs[] = {{"boundary.c", "end", (void *)0x400010, 32}};
  void *initial_rbp = NULL;
  void *stack = allocate_stack(funcs, 1, &initial_rbp);
  FILE *output = tmpfile();
  struct debugging_info dbg = {
      "boundary.c",
      "end",
      (void *)0x400000,
      (void *)0x400010,
  };

  print_backtrace(output, initial_rbp, &dbg, 1);

  fflush(output);
  fseek(output, 0, SEEK_SET);
  char buffer[1024] = {0};
  fread(buffer, 1, sizeof(buffer) - 1, output);
  fclose(output);
  free(stack);

  ASSERT(strstr(buffer, "end") != NULL);
  PASS();
}

TEST test_overlapping_debug_info_prioritizes_first(void) {
  struct function funcs[] = {{"shared.c", "target", (void *)0x400500, 32}};
  void *initial_rbp = NULL;
  void *stack = allocate_stack(funcs, 1, &initial_rbp);
  FILE *output = tmpfile();

  struct debugging_info dbg[] = {{
                                     "correct.c",
                                     "first",
                                     (void *)0x400000,
                                     (void *)0x400900,
                                 },
                                 {
                                     "wrong.c",
                                     "second",
                                     (void *)0x400000,
                                     (void *)0x400900,
                                 }};

  print_backtrace(output, initial_rbp, dbg, 2);

  fflush(output);
  fseek(output, 0, SEEK_SET);
  char buffer[1024] = {0};
  fread(buffer, 1, sizeof(buffer) - 1, output);
  fclose(output);
  free(stack);

  ASSERT(strstr(buffer, "correct.c") != NULL);
  ASSERT(strstr(buffer, "wrong.c") == NULL);
  PASS();
}

TEST test_large_stack_frames_offsets(void) {
  struct function funcs[] = {{"big.c", "base", (void *)0x100, 128},
                             {"big.c", "mid", (void *)0x200, 128}};
  size_t func_count = 2;
  char buffer[1024];
  run_test(buffer, sizeof(buffer), funcs, func_count);

  ASSERT(strstr(buffer, "mid") != NULL);
  ASSERT(strstr(buffer, "base") != NULL);
  PASS();
}

TEST test_alternating_frame_sizes(void) {
  struct function funcs[] = {{"alt.c", "f1", (void *)0x100, 8},
                             {"alt.c", "f2", (void *)0x200, 128},
                             {"alt.c", "f3", (void *)0x300, 16},
                             {"alt.c", "f4", (void *)0x400, 64}};
  size_t func_count = 4;
  char buffer[1024];
  run_test(buffer, sizeof(buffer), funcs, func_count);

  ASSERT(strstr(buffer, "f4") != NULL);
  ASSERT(strstr(buffer, "f3") != NULL);
  ASSERT(strstr(buffer, "f2") != NULL);
  ASSERT(strstr(buffer, "f1") != NULL);
  PASS();
}

SUITE(stack_traversal_suite) {
  RUN_TEST(test1);
  RUN_TEST(test_one_function);
  RUN_TEST(test_many_functions);
  RUN_TEST(test_max_stack_utilization);
  RUN_TEST(test_zero_frame_size);
  RUN_TEST(test_address_at_range_start);
  RUN_TEST(test_address_at_range_end);
  RUN_TEST(test_overlapping_debug_info_prioritizes_first);
  RUN_TEST(test_large_stack_frames_offsets);
  RUN_TEST(test_alternating_frame_sizes);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(stack_traversal_suite);
  GREATEST_PRINT_REPORT();
  custom_tests();
  return greatest_all_passed() ? EXIT_SUCCESS : EXIT_FAILURE;
}
