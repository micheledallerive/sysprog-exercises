#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "custom_tests.h"
#include "greatest.h"
#include "lib.h"

/* ------------------------------------------------------------
 * Helpers
 * ------------------------------------------------------------ */

static struct registers make_regs(void) {
  struct registers r;
  for (size_t i = 0; i < REG_INVALID; i++) {
    r.regs[i] = 0x1000 + (uint64_t)i * 0x10;
  }
  return r;
}

static uint64_t reg_val(enum reg r) {
  return 0x1000 + (uint64_t)r * 0x10;
}

static const char *reg_to_str(enum reg r) {
  switch (r) {
    case REG_RAX:
      return "%rax";
    case REG_RCX:
      return "%rcx";
    case REG_RDX:
      return "%rdx";
    case REG_RBX:
      return "%rbx";
    case REG_RSP:
      return "%rsp";
    case REG_RBP:
      return "%rbp";
    case REG_RSI:
      return "%rsi";
    case REG_RDI:
      return "%rdi";
    case REG_R8:
      return "%r8";
    case REG_R9:
      return "%r9";
    case REG_R10:
      return "%r10";
    case REG_R11:
      return "%r11";
    case REG_R12:
      return "%r12";
    case REG_R13:
      return "%r13";
    case REG_R14:
      return "%r14";
    case REG_R15:
      return "%r15";
    default:
      return "invalid";
  }
}

/* ------------------------------------------------------------
 * Tests: valid syntax / correct computation
 * ------------------------------------------------------------ */

TEST lea_base_only(void) {
  struct registers regs = make_regs();
  uintptr_t out;

  ASSERT(lea("(%rax)", &regs, &out));
  ASSERT_EQ(out, reg_val(REG_RAX));

  ASSERT(lea("(%rsp)", &regs, &out));
  ASSERT_EQ(out, reg_val(REG_RSP));

  ASSERT(lea("(%r15)", &regs, &out));
  ASSERT_EQ(out, reg_val(REG_R15));

  PASS();
}

TEST lea_disp_base(void) {
  struct registers regs = make_regs();
  uintptr_t out;

  ASSERT(lea("32(%rdi)", &regs, &out));
  ASSERT_EQ(out, reg_val(REG_RDI) + 32);

  ASSERT(lea("-16(%rbp)", &regs, &out));
  ASSERT_EQ(out, reg_val(REG_RBP) - 16);

  ASSERT(lea("0(%rax)", &regs, &out));
  ASSERT_EQ(out, reg_val(REG_RAX));

  PASS();
}

TEST lea_base_index_scale(void) {
  struct registers regs = make_regs();
  uintptr_t out;

  ASSERT(lea("(%rax,%rcx,1)", &regs, &out));
  ASSERT_EQ(out, reg_val(REG_RAX) + reg_val(REG_RCX));

  ASSERT(lea("(%rax,%rcx,2)", &regs, &out));
  ASSERT_EQ(out, reg_val(REG_RAX) + 2 * reg_val(REG_RCX));

  ASSERT(lea("(%rax,%rcx,4)", &regs, &out));
  ASSERT_EQ(out, reg_val(REG_RAX) + 4 * reg_val(REG_RCX));

  ASSERT(lea("(%rax,%rcx,8)", &regs, &out));
  ASSERT_EQ(out, reg_val(REG_RAX) + 8 * reg_val(REG_RCX));

  PASS();
}

TEST lea_full_form(void) {
  struct registers regs = make_regs();
  uintptr_t out;

  ASSERT(lea("32(%rdi,%rsi,4)", &regs, &out));
  ASSERT_EQ(out, 32 + reg_val(REG_RDI) + 4 * reg_val(REG_RSI));

  ASSERT(lea("-128(%r8,%r9,8)", &regs, &out));
  ASSERT_EQ(out, (uintptr_t)(-128) + reg_val(REG_R8) + 8 * reg_val(REG_R9));

  PASS();
}

TEST lea_disp_bounds(void) {
  struct registers regs = make_regs();
  uintptr_t out;

  ASSERT(lea("2147483647(%rax)", &regs, &out));
  ASSERT_EQ(out, reg_val(REG_RAX) + (int64_t)INT32_MAX);

  ASSERT(lea("-2147483648(%rax)", &regs, &out));
  ASSERT_EQ(out, reg_val(REG_RAX) + (int64_t)INT32_MIN);

  PASS();
}

/* ------------------------------------------------------------
 * Tests: invalid scale
 * ------------------------------------------------------------ */

TEST lea_invalid_scale(void) {
  struct registers regs = make_regs();
  uintptr_t out;

  ASSERT_FALSE(lea("(%rax,%rcx,0)", &regs, &out));
  ASSERT_FALSE(lea("(%rax,%rcx,3)", &regs, &out));
  ASSERT_FALSE(lea("(%rax,%rcx,5)", &regs, &out));
  ASSERT_FALSE(lea("(%rax,%rcx,16)", &regs, &out));
  ASSERT_FALSE(lea("(%rax,%rcx,-1)", &regs, &out));

  PASS();
}

/* ------------------------------------------------------------
 * Tests: invalid registers
 * ------------------------------------------------------------ */

TEST lea_invalid_registers(void) {
  struct registers regs = make_regs();
  uintptr_t out;

  ASSERT_FALSE(lea("(%foo)", &regs, &out));
  ASSERT_FALSE(lea("(%rax,%foo,4)", &regs, &out));
  ASSERT_FALSE(lea("(%r16)", &regs, &out));
  ASSERT_FALSE(lea("(%r-1)", &regs, &out));
  ASSERT_FALSE(lea("(%rax,%r17,2)", &regs, &out));

  PASS();
}

/* ------------------------------------------------------------
 * Tests: malformed syntax
 * ------------------------------------------------------------ */

TEST lea_malformed_strings(void) {
  struct registers regs = make_regs();
  uintptr_t out;

  ASSERT_FALSE(lea("", &regs, &out));
  ASSERT_FALSE(lea("()", &regs, &out));
  ASSERT_FALSE(lea("(%rax", &regs, &out));
  ASSERT_FALSE(lea("%rax)", &regs, &out));
  ASSERT_FALSE(lea("32%rax)", &regs, &out));
  ASSERT_FALSE(lea("32(%rax,%rcx)", &regs, &out));
  ASSERT_FALSE(lea("(%rax,,4)", &regs, &out));
  ASSERT_FALSE(lea("(%rax,%rcx,4,5)", &regs, &out));
  ASSERT_FALSE(lea("(%rax,%rcx)", &regs, &out));
  ASSERT_FALSE(lea("(,%rax,4)", &regs, &out));

  PASS();
}

/* ------------------------------------------------------------
 * Tests: arithmetic overflow / wraparound
 * ------------------------------------------------------------ */

TEST lea_overflow_behavior(void) {
  struct registers regs = make_regs();
  uintptr_t out;

  regs.regs[REG_RAX] = UINT64_MAX;
  regs.regs[REG_RCX] = 2;

  ASSERT(lea("(%rax,%rcx,8)", &regs, &out));
  ASSERT_EQ(out, UINT64_MAX + 16);

  PASS();
}

/* ------------------------------------------------------------
 * Tests: systematic register coverage
 * ------------------------------------------------------------ */

TEST lea_all_register_combinations(void) {
  struct registers regs = make_regs();
  uintptr_t out;
  char buf[64];

  for (int b = 0; b < REG_INVALID; b++) {
    snprintf(buf, sizeof(buf), "(%s)", reg_to_str((enum reg)b));
    ASSERT(lea(buf, &regs, &out));
    ASSERT_EQ(out, reg_val((enum reg)b));
  }

  for (int b = 0; b < REG_INVALID; b++) {
    for (int i = 0; i < REG_INVALID; i++) {
      snprintf(buf, sizeof(buf), "(%s,%s,1)", reg_to_str((enum reg)b),
               reg_to_str((enum reg)i));
      ASSERT(lea(buf, &regs, &out));
      ASSERT_EQ(out, reg_val((enum reg)b) + reg_val((enum reg)i));
    }
  }

  PASS();
}

/* ------------------------------------------------------------
 * Suite
 * ------------------------------------------------------------ */

SUITE(lea_suite) {
  RUN_TEST(lea_base_only);
  RUN_TEST(lea_disp_base);
  RUN_TEST(lea_base_index_scale);
  RUN_TEST(lea_full_form);
  RUN_TEST(lea_disp_bounds);
  RUN_TEST(lea_invalid_scale);
  RUN_TEST(lea_invalid_registers);
  RUN_TEST(lea_malformed_strings);
  RUN_TEST(lea_overflow_behavior);
  RUN_TEST(lea_all_register_combinations);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(lea_suite);
  GREATEST_PRINT_REPORT();
  custom_tests();
  return greatest_all_passed() ? EXIT_SUCCESS : EXIT_FAILURE;
}
