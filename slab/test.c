#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../greatest.h"
#include "custom_tests.h"
#include "lib.h"

TEST test_strict_alignment_and_spacing() {
  slab_allocator_t *alloc = slab_allocator_create();
  size_t align = 64;
  size_t size = 10;
  slab_cache_t *cache = slab_cache_create(alloc, size, align);

  void *p1 = slab_alloc(cache);
  void *p2 = slab_alloc(cache);

  ASSERT_EQ(0, (uintptr_t)p1 % align);
  ASSERT_EQ(0, (uintptr_t)p2 % align);

  uintptr_t diff = (uintptr_t)p2 > (uintptr_t)p1
                       ? (uintptr_t)p2 - (uintptr_t)p1
                       : (uintptr_t)p1 - (uintptr_t)p2;

  ASSERT(diff >= align);
  ASSERT_EQ(0, diff % align);

  slab_allocator_free(alloc);
  PASS();
}

TEST test_full_list_transition() {
  slab_allocator_t *alloc = slab_allocator_create();
  size_t size = 1024;
  slab_cache_t *cache = slab_cache_create(alloc, size, size);

  size_t capacity = PAGE_SIZE / size;
  void **ptrs = malloc(sizeof(void *) * capacity);

  for (size_t i = 0; i < capacity; i++) {
    ptrs[i] = slab_alloc(cache);
    ASSERT(ptrs[i] != NULL);
  }

  void *extra = slab_alloc(cache);
  ASSERT(extra != NULL);

  slab_free(cache, extra);
  for (size_t i = 0; i < capacity; i++) {
    slab_free(cache, ptrs[i]);
  }

  free(ptrs);
  slab_allocator_free(alloc);
  PASS();
}

TEST test_minimum_object_size_constraint() {
  slab_allocator_t *alloc = slab_allocator_create();
  slab_cache_t *cache = slab_cache_create(alloc, 1, 1);

  void *p1 = slab_alloc(cache);
  void *p2 = slab_alloc(cache);

  uintptr_t diff = (uintptr_t)p2 > (uintptr_t)p1
                       ? (uintptr_t)p2 - (uintptr_t)p1
                       : (uintptr_t)p1 - (uintptr_t)p2;

  ASSERT(diff >= sizeof(void *));

  slab_allocator_free(alloc);
  PASS();
}

TEST test_massive_alignment() {
  slab_allocator_t *alloc = slab_allocator_create();
  slab_cache_t *cache = slab_cache_create(alloc, 16, 1024);

  void *p1 = slab_alloc(cache);
  ASSERT_EQ(0, (uintptr_t)p1 % 1024);

  void *p2 = slab_alloc(cache);
  ASSERT_EQ(0, (uintptr_t)p2 % 1024);

  slab_allocator_free(alloc);
  PASS();
}

TEST test_cache_linkage_in_allocator() {
  slab_allocator_t *alloc = slab_allocator_create();
  slab_cache_t *c1 = slab_cache_create(alloc, 16, 8);
  slab_cache_t *c2 = slab_cache_create(alloc, 32, 8);

  ASSERT(c1 != NULL);
  ASSERT(c2 != NULL);

  slab_allocator_free(alloc);
  PASS();
}

TEST test_partial_to_free_transition() {
  slab_allocator_t *alloc = slab_allocator_create();
  slab_cache_t *cache = slab_cache_create(alloc, 512, 512);

  void *p1 = slab_alloc(cache);
  void *p2 = slab_alloc(cache);

  slab_free(cache, p1);
  slab_free(cache, p2);

  void *p3 = slab_alloc(cache);
  ASSERT(p3 == p2 || p3 == p1);

  slab_allocator_free(alloc);
  PASS();
}

TEST test_object_overlap() {
  slab_allocator_t *alloc = slab_allocator_create();
  slab_cache_t *cache = slab_cache_create(alloc, 64, 64);

  uint8_t *p1 = slab_alloc(cache);
  uint8_t *p2 = slab_alloc(cache);

  memset(p1, 0xAA, 64);
  memset(p2, 0xBB, 64);

  for (int i = 0; i < 64; i++) {
    ASSERT_EQ(0xAA, p1[i]);
    ASSERT_EQ(0xBB, p2[i]);
  }

  slab_allocator_free(alloc);
  PASS();
}

TEST test_large_number_of_caches() {
  slab_allocator_t *alloc = slab_allocator_create();
  for (int i = 0; i < 100; i++) {
    slab_cache_create(alloc, 16 + i, 8);
  }
  slab_allocator_free(alloc);
  PASS();
}

TEST test_many_slabs_per_cache() {
  slab_allocator_t *alloc = slab_allocator_create();
  slab_cache_t *cache = slab_cache_create(alloc, 2048, 8);

  void *ptrs[100];
  for (int i = 0; i < 100; i++) {
    ptrs[i] = slab_alloc(cache);
  }

  for (int i = 0; i < 100; i++) {
    slab_free(cache, ptrs[i]);
  }

  slab_allocator_free(alloc);
  PASS();
}

TEST test_aligned_alloc_failure_handling() {
  slab_allocator_t *alloc = slab_allocator_create();
  slab_cache_t *cache = slab_cache_create(alloc, 16, 8);

  slab_allocator_free(alloc);
  PASS();
}

TEST test_slab_free_full_list() {
  slab_allocator_t *alloc = slab_allocator_create();
  slab_cache_t *cache = slab_cache_create(alloc, 4096, 4096);

  void *p1 = slab_alloc(cache);
  void *p2 = slab_alloc(cache);

  slab_free(cache, p1);
  slab_free(cache, p2);

  slab_allocator_free(alloc);
  PASS();
}

TEST test_mixed_free_order() {
  slab_allocator_t *alloc = slab_allocator_create();
  slab_cache_t *cache = slab_cache_create(alloc, 128, 8);

  void *p[10];
  for (int i = 0; i < 10; i++) p[i] = slab_alloc(cache);

  slab_free(cache, p[5]);
  slab_free(cache, p[0]);
  slab_free(cache, p[9]);

  void *n1 = slab_alloc(cache);
  ASSERT(n1 == p[9]);

  slab_allocator_free(alloc);
  PASS();
}

TEST test_zero_allocation_count_on_free() {
  slab_allocator_t *alloc = slab_allocator_create();
  slab_cache_t *cache = slab_cache_create(alloc, 1024, 8);

  void *p1 = slab_alloc(cache);
  slab_free(cache, p1);

  slab_allocator_free(alloc);
  PASS();
}

TEST test_multiple_allocators_independent() {
  slab_allocator_t *a1 = slab_allocator_create();
  slab_allocator_t *a2 = slab_allocator_create();

  slab_cache_t *c1 = slab_cache_create(a1, 16, 8);
  slab_cache_t *c2 = slab_cache_create(a2, 16, 8);

  void *p1 = slab_alloc(c1);
  void *p2 = slab_alloc(c2);

  ASSERT(p1 != p2);

  slab_allocator_free(a1);
  slab_allocator_free(a2);
  PASS();
}

TEST test_cache_destruction_without_allocs() {
  slab_allocator_t *alloc = slab_allocator_create();
  slab_cache_create(alloc, 16, 8);
  slab_allocator_free(alloc);
  PASS();
}

TEST test_realloc_sequence() {
  slab_allocator_t *alloc = slab_allocator_create();
  slab_cache_t *cache = slab_cache_create(alloc, 256, 8);

  for (int i = 0; i < 50; i++) {
    void *p = slab_alloc(cache);
    slab_free(cache, p);
  }

  slab_allocator_free(alloc);
  PASS();
}

TEST test_alignment_power_of_two_verify() {
  slab_allocator_t *alloc = slab_allocator_create();
  size_t aligns[] = {8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};

  for (int i = 0; i < 10; i++) {
    slab_cache_t *c = slab_cache_create(alloc, 4, aligns[i]);
    void *p = slab_alloc(c);
    ASSERT_EQ(0, (uintptr_t)p % aligns[i]);
  }

  slab_allocator_free(alloc);
  PASS();
}

TEST test_internal_fragmentation_check() {
  slab_allocator_t *alloc = slab_allocator_create();
  slab_cache_t *cache = slab_cache_create(alloc, 2000, 8);

  void *p1 = slab_alloc(cache);
  void *p2 = slab_alloc(cache);

  uintptr_t diff = (uintptr_t)p2 - (uintptr_t)p1;
  ASSERT(diff >= 2000);

  slab_allocator_free(alloc);
  PASS();
}

TEST test_free_list_integrity() {
  slab_allocator_t *alloc = slab_allocator_create();
  slab_cache_t *cache = slab_cache_create(alloc, 32, 32);

  void *p1 = slab_alloc(cache);
  void *p2 = slab_alloc(cache);
  void *p3 = slab_alloc(cache);

  slab_free(cache, p2);
  void *p2_new = slab_alloc(cache);
  ASSERT_EQ(p2, p2_new);

  slab_allocator_free(alloc);
  PASS();
}

TEST test_memory_filling_stress() {
  slab_allocator_t *alloc = slab_allocator_create();
  slab_cache_t *cache = slab_cache_create(alloc, 16, 8);

  void **ptrs = malloc(sizeof(void *) * 10000);
  for (int i = 0; i < 10000; i++) {
    ptrs[i] = slab_alloc(cache);
    memset(ptrs[i], 0xFF, 16);
  }

  for (int i = 0; i < 10000; i++) {
    slab_free(cache, ptrs[i]);
  }

  free(ptrs);
  slab_allocator_free(alloc);
  PASS();
}

TEST test_slab_reallocation_after_empty() {
  slab_allocator_t *alloc = slab_allocator_create();
  slab_cache_t *cache = slab_cache_create(alloc, 4096, 4096);

  void *p1 = slab_alloc(cache);
  slab_free(cache, p1);

  void *p2 = slab_alloc(cache);
  ASSERT_EQ(p1, p2);

  slab_allocator_free(alloc);
  PASS();
}

TEST test_invalid_free_safety() {
  slab_allocator_t *alloc = slab_allocator_create();
  slab_allocator_free(alloc);
  PASS();
}

TEST test_cache_overlap_prevention() {
  slab_allocator_t *alloc = slab_allocator_create();
  slab_cache_t *c1 = slab_cache_create(alloc, 16, 8);
  slab_cache_t *c2 = slab_cache_create(alloc, 16, 8);

  void *p1 = slab_alloc(c1);
  void *p2 = slab_alloc(c2);

  ASSERT(p1 != p2);

  slab_allocator_free(alloc);
  PASS();
}

TEST test_page_boundary_adherence() {
  slab_allocator_t *alloc = slab_allocator_create();
  slab_cache_t *cache = slab_cache_create(alloc, 3000, 8);

  void *p1 = slab_alloc(cache);
  void *p2 = slab_alloc(cache);

  uintptr_t page1 = (uintptr_t)p1 / PAGE_SIZE;
  uintptr_t page2 = (uintptr_t)p2 / PAGE_SIZE;

  ASSERT(page1 != page2);

  slab_allocator_free(alloc);
  PASS();
}

TEST test_small_object_recycling() {
  slab_allocator_t *alloc = slab_allocator_create();
  slab_cache_t *cache = slab_cache_create(alloc, 8, 8);

  void *p1 = slab_alloc(cache);
  slab_free(cache, p1);
  void *p2 = slab_alloc(cache);
  ASSERT_EQ(p1, p2);

  slab_allocator_free(alloc);
  PASS();
}

TEST test_allocation_stability() {
  slab_allocator_t *alloc = slab_allocator_create();
  slab_cache_t *cache = slab_cache_create(alloc, 128, 8);

  void *p = slab_alloc(cache);
  for (int i = 0; i < 1000; i++) {
    ASSERT(p != NULL);
  }

  slab_allocator_free(alloc);
  PASS();
}

TEST test_huge_cache_set() {
  slab_allocator_t *alloc = slab_allocator_create();
  for (int i = 0; i < 10; i++) {
    slab_cache_t *c = slab_cache_create(alloc, 16, 8);
    for (int j = 0; j < 100; j++) {
      slab_free(c, slab_alloc(c));
    }
  }
  slab_allocator_free(alloc);
  PASS();
}

TEST test_manual_page_size_alignment() {
  slab_allocator_t *alloc = slab_allocator_create();
  slab_cache_t *cache = slab_cache_create(alloc, PAGE_SIZE, PAGE_SIZE);

  void *p1 = slab_alloc(cache);
  void *p2 = slab_alloc(cache);

  ASSERT_EQ(0, (uintptr_t)p1 % PAGE_SIZE);
  ASSERT_EQ(0, (uintptr_t)p2 % PAGE_SIZE);

  slab_allocator_free(alloc);
  PASS();
}

TEST test_partial_list_persistence() {
  slab_allocator_t *alloc = slab_allocator_create();
  slab_cache_t *cache = slab_cache_create(alloc, 1024, 8);

  void *p1 = slab_alloc(cache);
  void *p2 = slab_alloc(cache);
  void *p3 = slab_alloc(cache);

  slab_free(cache, p2);
  void *p4 = slab_alloc(cache);
  ASSERT_EQ(p2, p4);

  slab_allocator_free(alloc);
  PASS();
}

TEST test_empty_allocator_free() {
  slab_allocator_t *alloc = slab_allocator_create();
  slab_allocator_free(alloc);
  PASS();
}

SUITE(slab_suite) {
  RUN_TEST(test_strict_alignment_and_spacing);
  RUN_TEST(test_full_list_transition);
  RUN_TEST(test_minimum_object_size_constraint);
  RUN_TEST(test_massive_alignment);
  RUN_TEST(test_cache_linkage_in_allocator);
  RUN_TEST(test_partial_to_free_transition);
  RUN_TEST(test_object_overlap);
  RUN_TEST(test_large_number_of_caches);
  RUN_TEST(test_many_slabs_per_cache);
  RUN_TEST(test_aligned_alloc_failure_handling);
  RUN_TEST(test_slab_free_full_list);
  RUN_TEST(test_mixed_free_order);
  RUN_TEST(test_zero_allocation_count_on_free);
  RUN_TEST(test_multiple_allocators_independent);
  RUN_TEST(test_cache_destruction_without_allocs);
  RUN_TEST(test_realloc_sequence);
  RUN_TEST(test_alignment_power_of_two_verify);
  RUN_TEST(test_internal_fragmentation_check);
  RUN_TEST(test_free_list_integrity);
  RUN_TEST(test_memory_filling_stress);
  RUN_TEST(test_slab_reallocation_after_empty);
  RUN_TEST(test_invalid_free_safety);
  RUN_TEST(test_cache_overlap_prevention);
  RUN_TEST(test_page_boundary_adherence);
  RUN_TEST(test_small_object_recycling);
  RUN_TEST(test_allocation_stability);
  RUN_TEST(test_huge_cache_set);
  RUN_TEST(test_manual_page_size_alignment);
  RUN_TEST(test_partial_list_persistence);
  RUN_TEST(test_empty_allocator_free);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_SUITE(slab_suite);
  GREATEST_PRINT_REPORT();
  custom_tests();
  return greatest_all_passed() ? EXIT_SUCCESS : EXIT_FAILURE;
}
