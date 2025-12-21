#ifndef LIB_H
#define LIB_H

#include <stddef.h>
#include <stdio.h>

struct debugging_info {
  const char *file_name;
  const char *function_name;
  void *start_addr;
  void *end_addr;
};

/**
 * Inspect the stack to identify the backtrace of function calls.
 * Use the debug information to print filename and function name for each caller
 * function found on the stack. The information should be written to the given
 * file stream.
 *
 * The output is expected in the following format, starting from the most recent
 * function call, one per line:
 *
 * <return_address> <file_name>:<function_name>
 */
void print_backtrace(FILE *file, void *current_rbp,
                     const struct debugging_info *dbg_info_array,
                     size_t dbg_info_count);

#endif  // LIB_H
