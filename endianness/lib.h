#ifndef LIB_H
#define LIB_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#undef __ORDER_BIG_ENDIAN__
#undef __ORDER_LITTLE_ENDIAN__
#undef __BYTE_ORDER__
#undef BIG_ENDIAN
#undef LITTLE_ENDIAN
#undef BYTE_ORDER

typedef enum { LITTLE_ENDIAN, BIG_ENDIAN, OTHER_ENDIAN } endianness_t;

endianness_t get_system_endianness();

uint16_t swap_u16(uint16_t val);
uint32_t swap_u32(uint32_t val);
uint64_t swap_u64(uint64_t val);

#endif  // LIB_H
