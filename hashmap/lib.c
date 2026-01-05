#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"

// You might find a struct `hashmap_node` useful

struct hashmap {
  // Implement internal state
};

struct hashmap_iterator {
  // Implement iterator state
};

hashmap_t *hashmap_create(size_t num_buckets, size_t key_size,
                          size_t value_size) {
  return NULL;
}

void hashmap_free(hashmap_t *map) {}

bool hashmap_put(hashmap_t *map, void *key, void *value) {
  return false;
}

bool hashmap_get(const hashmap_t *map, const void *key, void **out_value) {
  return false;
}

bool hashmap_contains(const hashmap_t *map, const void *key) {
  return false;
}

void hashmap_remove(hashmap_t *map, const void *key) {}

size_t hashmap_size(const hashmap_t *map) {
  return 0;
}

void hashmap_clear(hashmap_t *map) {}

float hashmap_load_factor(const hashmap_t *map) {
  return 0.0f;
}

hashmap_iterator_t *hashmap_iterator_create(const hashmap_t *map) {
  return NULL;
}
void hashmap_iterator_free(hashmap_iterator_t *iter) {}

bool hashmap_iterator_next(hashmap_iterator_t *iter) {
  return false;
}

const void *hashmap_iterator_key(const hashmap_iterator_t *iter) {
  return NULL;
}

void *hashmap_iterator_value(const hashmap_iterator_t *iter) {
  return NULL;
}
