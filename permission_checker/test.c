#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "custom_tests.h"
#include "greatest.h"
#include "lib.h"

TEST test_read_success() {
  struct page_table pt = {0};
  pa_t physical_address = 0;
  pt.entries[0] = (1 << 12) | (1 << 13) | 0xABC;
  result_t res = access_memory(&pt, 0x00000042, READ, &physical_address);
  ASSERT_EQ(SUCCESS, res);
  ASSERT_EQ(0xABC42, physical_address);
  PASS();
}

TEST test_write_success() {
  struct page_table pt = {0};
  pa_t physical_address = 0;
  pt.entries[5] = (1 << 12) | (1 << 14) | 0x123;
  result_t res = access_memory(&pt, (5 << 8) | 0x1F, WRITE, &physical_address);
  ASSERT_EQ(SUCCESS, res);
  ASSERT_EQ(0x1231F, physical_address);
  PASS();
}

TEST test_execute_success() {
  struct page_table pt = {0};
  pa_t physical_address = 0;
  pt.entries[10] = (1 << 12) | (1 << 15) | 0x777;
  result_t res =
      access_memory(&pt, (10 << 8) | 0x00, EXECUTE, &physical_address);
  ASSERT_EQ(SUCCESS, res);
  ASSERT_EQ(0x77700, physical_address);
  PASS();
}

TEST test_page_fault_invalid_bit() {
  struct page_table pt = {0};
  pa_t physical_address = 0;
  pt.entries[0] = (1 << 13) | (1 << 14) | (1 << 15) | 0xABC;
  result_t res = access_memory(&pt, 0x0000, READ, &physical_address);
  ASSERT_EQ(PAGE_FAULT, res);
  PASS();
}

TEST test_read_denied() {
  struct page_table pt = {0};
  pa_t physical_address = 0;
  pt.entries[1] = (1 << 12) | (1 << 14) | (1 << 15);
  result_t res = access_memory(&pt, (1 << 8), READ, &physical_address);
  ASSERT_EQ(PERMISSION_DENIED, res);
  PASS();
}

TEST test_write_denied() {
  struct page_table pt = {0};
  pa_t physical_address = 0;
  pt.entries[1] = (1 << 12) | (1 << 13) | (1 << 15);
  result_t res = access_memory(&pt, (1 << 8), WRITE, &physical_address);
  ASSERT_EQ(PERMISSION_DENIED, res);
  PASS();
}

TEST test_execute_denied() {
  struct page_table pt = {0};
  pa_t physical_address = 0;
  pt.entries[1] = (1 << 12) | (1 << 13) | (1 << 14);
  result_t res = access_memory(&pt, (1 << 8), EXECUTE, &physical_address);
  ASSERT_EQ(PERMISSION_DENIED, res);
  PASS();
}

TEST test_max_vpn_mapping() {
  struct page_table pt = {0};
  pa_t physical_address = 0;
  size_t last_vpn = (1 << VPN_BITS) - 1;
  pt.entries[last_vpn] = (1 << 12) | (1 << 13) | 0xFFF;
  result_t res =
      access_memory(&pt, (last_vpn << 8) | 0xFF, READ, &physical_address);
  ASSERT_EQ(SUCCESS, res);
  ASSERT_EQ(0xFFFFF, physical_address);
  PASS();
}

TEST test_all_permissions_set() {
  struct page_table pt = {0};
  pa_t physical_address = 0;
  pt.entries[100] = (1 << 12) | (1 << 13) | (1 << 14) | (1 << 15) | 0x1;
  ASSERT_EQ(SUCCESS, access_memory(&pt, (100 << 8), READ, &physical_address));
  ASSERT_EQ(SUCCESS, access_memory(&pt, (100 << 8), WRITE, &physical_address));
  ASSERT_EQ(SUCCESS,
            access_memory(&pt, (100 << 8), EXECUTE, &physical_address));
  PASS();
}

TEST test_offset_preservation() {
  struct page_table pt = {0};
  pa_t physical_address = 0;
  pt.entries[0x20] = (1 << 12) | (1 << 13) | 0x50;
  for (int i = 0; i < 256; i++) {
    access_memory(&pt, (0x20 << 8) | i, READ, &physical_address);
    ASSERT_EQ((0x50 << 8) | i, physical_address);
  }
  PASS();
}

TEST test_zero_ppn_mapping() {
  struct page_table pt = {0};
  pa_t physical_address = 0;
  pt.entries[50] = (1 << 12) | (1 << 13) | 0x000;
  result_t res = access_memory(&pt, (50 << 8) | 0xAA, READ, &physical_address);
  ASSERT_EQ(SUCCESS, res);
  ASSERT_EQ(0x000AA, physical_address);
  PASS();
}

TEST test_vpn_extraction_middle_bits() {
  struct page_table pt = {0};
  pa_t physical_address = 0;
  pt.entries[0x1234] = (1 << 12) | (1 << 13) | 0xAAA;
  result_t res = access_memory(&pt, (0x1234 << 8), READ, &physical_address);
  ASSERT_EQ(SUCCESS, res);
  ASSERT_EQ(0xAAA00, physical_address);
  PASS();
}

TEST test_no_permissions_granted() {
  struct page_table pt = {0};
  pa_t physical_address = 0;
  pt.entries[5] = (1 << 12);
  ASSERT_EQ(PERMISSION_DENIED,
            access_memory(&pt, (5 << 8), READ, &physical_address));
  ASSERT_EQ(PERMISSION_DENIED,
            access_memory(&pt, (5 << 8), WRITE, &physical_address));
  ASSERT_EQ(PERMISSION_DENIED,
            access_memory(&pt, (5 << 8), EXECUTE, &physical_address));
  PASS();
}

TEST test_large_va_truncation() {
  struct page_table pt = {0};
  pa_t physical_address = 0;
  pt.entries[0] = (1 << 12) | (1 << 13) | 0x1;
  result_t res = access_memory(&pt, (1 << 22), READ, &physical_address);
  ASSERT_EQ(SUCCESS, res);
  ASSERT_EQ(0x100, physical_address);
  PASS();
}

TEST test_ppn_boundaries() {
  struct page_table pt = {0};
  pa_t physical_address = 0;
  pt.entries[1] = (1 << 12) | (1 << 13) | 0x800;
  access_memory(&pt, (1 << 8), READ, &physical_address);
  ASSERT_EQ(0x80000, physical_address);
  PASS();
}

TEST test_consecutive_vpns() {
  struct page_table pt = {0};
  pa_t physical_address = 0;
  pt.entries[10] = (1 << 12) | (1 << 13) | 0xA;
  pt.entries[11] = (1 << 12) | (1 << 13) | 0xB;
  access_memory(&pt, (10 << 8), READ, &physical_address);
  ASSERT_EQ(0xA00, physical_address);
  access_memory(&pt, (11 << 8), READ, &physical_address);
  ASSERT_EQ(0xB00, physical_address);
  PASS();
}

TEST test_permission_isolation_read_only() {
  struct page_table pt = {0};
  pa_t physical_address = 0;
  pt.entries[0] = (1 << 12) | (1 << 13);
  ASSERT_EQ(SUCCESS, access_memory(&pt, 0, READ, &physical_address));
  ASSERT_EQ(PERMISSION_DENIED, access_memory(&pt, 0, WRITE, &physical_address));
  ASSERT_EQ(PERMISSION_DENIED,
            access_memory(&pt, 0, EXECUTE, &physical_address));
  PASS();
}

TEST test_permission_isolation_write_only() {
  struct page_table pt = {0};
  pa_t physical_address = 0;
  pt.entries[0] = (1 << 12) | (1 << 14);
  ASSERT_EQ(PERMISSION_DENIED, access_memory(&pt, 0, READ, &physical_address));
  ASSERT_EQ(SUCCESS, access_memory(&pt, 0, WRITE, &physical_address));
  ASSERT_EQ(PERMISSION_DENIED,
            access_memory(&pt, 0, EXECUTE, &physical_address));
  PASS();
}

TEST test_permission_isolation_execute_only() {
  struct page_table pt = {0};
  pa_t physical_address = 0;
  pt.entries[0] = (1 << 12) | (1 << 15);
  ASSERT_EQ(PERMISSION_DENIED, access_memory(&pt, 0, READ, &physical_address));
  ASSERT_EQ(PERMISSION_DENIED, access_memory(&pt, 0, WRITE, &physical_address));
  ASSERT_EQ(SUCCESS, access_memory(&pt, 0, EXECUTE, &physical_address));
  PASS();
}

SUITE(permchecker_suite) {
  RUN_TEST(test_read_success);
  RUN_TEST(test_write_success);
  RUN_TEST(test_execute_success);
  RUN_TEST(test_page_fault_invalid_bit);
  RUN_TEST(test_read_denied);
  RUN_TEST(test_write_denied);
  RUN_TEST(test_execute_denied);
  RUN_TEST(test_max_vpn_mapping);
  RUN_TEST(test_all_permissions_set);
  RUN_TEST(test_offset_preservation);
  RUN_TEST(test_zero_ppn_mapping);
  RUN_TEST(test_vpn_extraction_middle_bits);
  RUN_TEST(test_no_permissions_granted);
  RUN_TEST(test_large_va_truncation);
  RUN_TEST(test_ppn_boundaries);
  RUN_TEST(test_consecutive_vpns);
  RUN_TEST(test_permission_isolation_read_only);
  RUN_TEST(test_permission_isolation_write_only);
  RUN_TEST(test_permission_isolation_execute_only);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(permchecker_suite);
  GREATEST_PRINT_REPORT();
  custom_tests();
  return greatest_all_passed() ? EXIT_SUCCESS : EXIT_FAILURE;
}
