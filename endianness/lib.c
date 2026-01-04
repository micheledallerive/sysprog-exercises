#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"

endianness_t get_system_endianness() {
  return OTHER_ENDIAN;
}

uint16_t swap_u16(uint16_t val) {
  return val;
}

uint32_t swap_u32(uint32_t val) {
  return val;
}

uint64_t swap_u64(uint64_t val) {
  return val;
}
