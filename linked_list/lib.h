#ifndef LIB_H
#define LIB_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct list_node {
  struct list_node *next;
  struct list_node *prev;
  int value;
};

typedef struct list_node list_node_t;

struct list {
  list_node_t *head;
  list_node_t *tail;
};

typedef struct list list_t;

static inline list_node_t *list_node_create(int value) {
  list_node_t *node = (list_node_t *)malloc(sizeof(list_node_t));
  if (!node) {
    return NULL;
  }
  node->value = value;
  node->prev = NULL;
  node->next = NULL;
  return node;
}

static inline void list_node_free(list_node_t *node) { free(node); }

static inline list_t *list_create() {
  list_t *list = (list_t *)malloc(sizeof(list_t));
  if (!list) {
    return NULL;
  }
  list->head = NULL;
  list->tail = NULL;
  return list;
}

static inline void list_free(list_t *list) {
  list_node_t *node = list->head;
  while (node) {
    list_node_t *next = node->next;
    list_node_free(node);
    node = next;
  }
  free(list);
}

static inline void list_sanity_check(list_t *list) {
  if (list->head == NULL && list->tail == NULL) {
    return;
  }
  assert(list->head != NULL && list->tail != NULL);
  assert(list->head->prev == NULL);
  assert(list->tail->next == NULL);
  list_node_t *curr = list->head;
  list_node_t *prev = NULL;
  while (curr) {
    assert(curr->prev == prev);
    prev = curr;
    curr = curr->next;
  }
  assert(prev == list->tail);
}

static inline void list_print(const list_t *list) {
  list_node_t *curr = list->head;
  printf("[");
  while (curr) {
    printf("%d", curr->value);
    curr = curr->next;
    if (curr) {
      printf(", ");
    }
  }
  printf("]\n");
}

/**
 * Return the length of the list.
 */
size_t list_length(const list_t *list);
/**
 * Check if the list is empty.
 */
bool list_empty(const list_t *list);

/**
 * Insert `node` after the `after` node in the list.
 * `after` must be a node in the list, and cannot be NULL.
 */
void list_insert_after(list_t *list, list_node_t *after, list_node_t *node);
/**
 * Insert `node` before the `before` node in the list.
 * `before` must be a node in the list, and cannot be NULL.
 */
void list_insert_before(list_t *list, list_node_t *before, list_node_t *node);

/**
 * Add an element to the front of the list.
 */
void list_push_front(list_t *list, list_node_t *node);
/**
 * Add an element to the back of the list.
 */
void list_push_back(list_t *list, list_node_t *node);

/**
 * Return the front node of the list.
 */
list_node_t *list_front(list_t *list);
/**
 * Return the back node of the list.
 */
list_node_t *list_back(list_t *list);

/**
 * Remove the front node of the list.
 * The node must be freed inside the function.
 * The list must not be empty.
 */
void list_pop_front(list_t *list);
/**
 * Remove the back node of the list.
 * The node must be freed inside the function.
 * The list must not be empty.
 */
void list_pop_back(list_t *list);

/**
 * Remove the specified node from the list.
 * The node must be freed inside the function.
 */
void list_remove(list_t *list, list_node_t *node);

/**
 * Merge two sorted lists `dest` and `src` into `dest`.
 * After the operation, `src` must be an empty list, and all the nodes
 * from `src` are moved to `dest`, maintaining the sorted order.
 *
 * For example, dest = [1,3,5], src = [2,4,6]
 * After list_merge(dest, src): dest = [1,2,3,4,5,6], src = []
 *
 * If either list is not sorted, the behavior is undefined.
 */
void list_merge(list_t *dest, list_t *src);
/**
 * Sort the list in-place in ascending order.
 */
void list_sort(list_t *list);
/**
 * Reverse the list in-place.
 */
void list_reverse(list_t *list);
/**
 * Remove consecutive duplicate elements from the list.
 * If duplicates are not consecutive, the behavior is undefined.
 *
 * For example, list = [1,1,2,3,3,3,4]
 * After list_unique(list): list = [1,2,3,4]
 */
void list_unique(list_t *list);

#endif  // LIB_H
