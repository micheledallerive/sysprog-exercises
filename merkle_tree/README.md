# Merkle Tree Implementation

## Goal

Implement a **Merkle Tree**, a fundamental data structure in cryptography and blockchain technology used to efficiently verify the integrity of large datasets. This exercise focuses on hierarchical hashing, iterative tree construction, and difference detection.

You must implement the library functions defined in `lib.h`, primarily the `merkle_tree_create()` and `merkle_tree_first_diff()` functions.

---

## The Problem

A (Merkle Tree)[https://en.wikipedia.org/wiki/Merkle_tree] is a binary tree where every leaf node contains the hash of a data block, and every non-leaf node contains the cryptographic hash of its children's hashes.

### Data Structures

The tree is built from binary data divided into chunks of `BLOCK_SIZE` (32 bytes).

* **`merkle_tree_node_t`**: A structure containing the node hash, type (leaf or inner), and pointers to children or data blocks.
* **`merkle_tree_t`**: A simple wrapper containing the pointer to the root node.

### Core Functionalities

```c
merkle_tree_t *merkle_tree_create(const void *data, size_t data_size);
int64_t merkle_tree_first_diff(const merkle_tree_t *a, const merkle_tree_t *b);
bool merkle_tree_verify(const merkle_tree_t *expected_tree, const void *data, size_t data_size);
```

---

## Constraints and Requirements

* **Tree Construction**: The tree should be built level-by-level (bottom-up). If a level has an odd number of nodes, the last node's hash is combined with itself (duplicated) to create the parent hash.
* **Hashing**: Use the `hash()` and `combine_hash()` functions that are provided in `lib.h`.
* **Difference Detection**: `merkle_tree_first_diff` must return the byte offset of the first data block that differs between two trees. If the trees are identical, it returns `-1`.
* **Memory Management**: You are responsible for recursively freeing all nodes when `merkle_tree_free` is called. Ensure no memory leaks occur during verification or tree updates.
* **Rehashing**: Implement `merkle_tree_rehash` to recalculate hashes if the underlying data blocks are modified externally.

---

## Testing Your Code

The provided test suite includes 48 test cases covering:
* Empty trees and single-byte inputs.
* Trees with even and odd numbers of leaf blocks.
* Integrity verification and tamper detection.
* Finding the exact offset of differences in large trees.
* Memory corruption and edge-case validation.

To run the tests:

```bash
make
./test
```

or

```bash
make run
```

You should see:

```text
* Suite merkle_tree_suite:
..............................

48 tests - 48 pass, 0 fail, 0 skipped
```

---

## Files You'll Modify

* **`lib.c`**: Implement the Merkle tree logic, including creation, freeing, and diffing.

## Files Provided

* **`lib.h`**: Data structure definitions and function prototypes.
* **`greatest.h`**: The unit testing framework.
* **`Makefile`**: Build instructions.
