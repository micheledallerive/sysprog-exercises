#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "lib.h"

merkle_tree_t *merkle_tree_create(const void *data, size_t data_size) {
  return NULL;
}

void merkle_tree_free(merkle_tree_t *tree) {}

hash_t merkle_tree_root_hash(merkle_tree_t *tree) {
  return 0;
}

void merkle_tree_rehash(merkle_tree_t *tree) {}

int64_t merkle_tree_first_diff(const merkle_tree_t *a, const merkle_tree_t *b) {
  return -1;
}
bool merkle_tree_verify(const merkle_tree_t *expected_tree, const void *data,
                        size_t data_size) {
  return true;
}
