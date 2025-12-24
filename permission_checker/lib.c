#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stdint.h>

#include "lib.h"

result_t access_memory(struct page_table *pt, va_t virtual_address,
                       action_type_t action, pa_t *physical_address) {
  return SUCCESS;
}
