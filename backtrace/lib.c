#include <assert.h>
#include <stdalign.h>
#include <stdint.h>
#include <stdio.h>

#include "lib.h"

void print_backtrace(FILE *file, void *current_rbp,
                     const struct debugging_info *dbg_info_array,
                     size_t dbg_info_count) {
  // Implement...
}
