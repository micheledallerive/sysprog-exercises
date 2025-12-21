# Doubly Linked List Operations

## Goal

Implement a robust **doubly linked list** library. This exercise focuses on pointer manipulation, list traversal, and fundamental algorithms like sorting and merging.

You must implement the core logic for managing `list_t` in `lib.c`. This includes:
1. **Basic Management**: Calculating length, checking if empty, and retrieving head/tail nodes.
2. **Insertion/Removal**: Adding or removing nodes at the front, back, or relative to specific nodes.
3. **Algorithms**: Sorting the list, reversing it in-place, merging two sorted lists, and removing consecutive duplicates.

---

## The Data Structure

The list is composed of nodes containing an integer value and pointers to both the next and previous elements:

```c
struct list_node {
  struct list_node *next;
  struct list_node *prev;
  int value;
};

struct list {
  list_node_t *head;
  list_node_t *tail;
};
```

A properly maintained list must always ensure that:
* If empty, both `head` and `tail` are `NULL`.
* If non-empty, `head->prev` is `NULL`, `tail->next` is `NULL` and if a node A has `A->next = B`, then `B->prev = A`.

If you want to check the integrity of your list, you can use the provided `list_sanity_check()` function.

Make sure to maintain the head and tail pointers correctly during insertion and removal operations.

---

## Testing Your Code

Run the test suite:

```bash
make
./test
```

or

```bash
make run
```

The tests verify your implementation using a series of assertions and a sanity check function that ensures the integrity of the `prev` and `next` pointer chain.

---

## Files You'll Modify

* **`lib.c`**: Implement all the functions declared in `lib.h`. Some helper functions are provided for you to use.

## Files Provided

* **`lib.h`**: Data structures, helper function definitions (create/free), and function prototypes.
* **`test.c`**: The testing suite.
* **`Makefile`**: Build instructions.
