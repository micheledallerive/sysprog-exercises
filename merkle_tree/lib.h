#ifndef LIB_H
#define LIB_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define BLOCK_SIZE 128

typedef uint64_t hash_t;

typedef struct merkle_tree_node {
  bool is_leaf;
  hash_t hash;
  size_t subtree_bytes;
  union {
    struct {  // Leaf
      const void *block;
      size_t size;
    };
    struct {  // Inner
      struct merkle_tree_node *left;
      struct merkle_tree_node *right;
    };
  };
} merkle_tree_node_t;

typedef struct {
  merkle_tree_node_t *root;
} merkle_tree_t;

static inline hash_t hash(const void *data, size_t size) {
  const unsigned char *p = (const unsigned char *)data;
  uint64_t h = 14695981039346656037ULL;

  for (size_t i = 0; i < size; i++) {
    h ^= p[i];
    h *= 1099511628211ULL;
  }

  return h;
}

static inline hash_t combine_hash(hash_t left, hash_t right) {
  hash_t data[2] = {left, right};
  return hash(data, sizeof(data));
}

merkle_tree_t *merkle_tree_create(const void *data, size_t data_size);
void merkle_tree_free(merkle_tree_t *tree);
/**
 * Return the root hash of the Merkle tree.
 */
hash_t merkle_tree_root_hash(merkle_tree_t *tree);
/**
 * Recompute all hashes in the Merkle tree.
 */
void merkle_tree_rehash(merkle_tree_t *tree);

/**
 * Return the byte offset of the first differing *block* between two Merkle
 * trees, or -1 if they are identical.
 */
int64_t merkle_tree_first_diff(const merkle_tree_t *a, const merkle_tree_t *b);
/**
 * Verify that the provided data matches the expected Merkle tree.
 */
bool merkle_tree_verify(const merkle_tree_t *expected_tree, const void *data,
                        size_t data_size);

#endif  // LIB_H
