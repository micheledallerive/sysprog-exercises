#ifndef LIB_H
#define LIB_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef uint8_t scale_t;
typedef int32_t displacement_t;

enum reg : uint8_t {
  REG_RAX = 0U,
  REG_RCX,
  REG_RDX,
  REG_RBX,
  REG_RSP,
  REG_RBP,
  REG_RSI,
  REG_RDI,
  REG_R8,
  REG_R9,
  REG_R10,
  REG_R11,
  REG_R12,
  REG_R13,
  REG_R14,
  REG_R15,
  REG_INVALID
};

static inline enum reg reg_invalid() {
  return (enum reg)(REG_INVALID);
}
static inline bool reg_is_invalid(enum reg r) {
  return r >= REG_INVALID;
}
static inline uint8_t reg_to_idx(enum reg r) {
  assert(r < REG_INVALID);
  return (uint8_t)r;
}

struct registers {
  uint64_t regs[REG_INVALID];
};

bool lea(const char *effective_address, struct registers *regs, uintptr_t *out);

#endif  // LIB_H
