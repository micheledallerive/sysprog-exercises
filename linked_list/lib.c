#include <assert.h>
#include <stdalign.h>
#include <stdint.h>
#include <stdio.h>

#include "lib.h"

size_t list_length(const list_t *list) { return 0; }

bool list_empty(const list_t *list) { return false; }

void list_insert_after(list_t *list, list_node_t *after, list_node_t *node) {}

void list_insert_before(list_t *list, list_node_t *before, list_node_t *node) {}

void list_push_front(list_t *list, list_node_t *node) {}

void list_push_back(list_t *list, list_node_t *node) {}

list_node_t *list_front(list_t *list) { return NULL; }
list_node_t *list_back(list_t *list) { return NULL; }

void list_pop_front(list_t *list) {}

void list_pop_back(list_t *list) {}

void list_remove(list_t *list, list_node_t *node) {}

void list_merge(list_t *dest, list_t *src) {}

void list_sort(list_t *list) {}

void list_reverse(list_t *list) {}
void list_unique(list_t *list) {}
