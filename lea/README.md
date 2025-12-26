# LEA Instruction Simulator

## Goal

Implement the address calculation logic for the x86_64 **LEA (Load Effective Address)** instruction. Unlike other instructions that access memory, LEA calculates the address and stores it in a destination. In this exercise, you will parse a string representation of an effective address and compute the resulting pointer using a simulated register set.

Implement the `lea()` function in `lib.c`. Your function must parse the input string, look up the values in the provided `registers` struct, and calculate the final address using the formula:

$$Address = Base + (Index \times Scale) + Displacement$$

---

## Supported Address Formats

Your parser must support a subset of x86_64 AT&T syntax. For this exercise, assume the input string contains **no spaces** and follows one of these four specific patterns:

* **(B)**: Base register only.
* **D(B)**: Displacement and Base register.
* **(B,I,S)**: Base, Index, and Scale.
* **D(B,I,S)**: Displacement, Base, Index, and Scale.

**Constraints:**
* **Registers (B, I)**: Must be one of the 16 standard 64-bit registers (e.g., `%rax`, `%rsp`, `%r12`).
* **Displacement (D)**: An integer (positive or negative).
* **Scale (S)**: Must be exactly **1**, **2**, **4**, or **8**.
* **Whitespace**: The input string will not contain spaces.

---

## The Data Structures

You are provided abstractions over registers and their values during execution:
```c

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

struct registers {
  uint64_t regs[REG_INVALID];
};
```

You must implement:

```c
bool lea(const char *effective_address, struct registers *regs, uintptr_t *out);
```

The function should return `true` if the address was successfully parsed and calculated, or `false` if the format is invalid.

---

## Testing Your Code

Build and run the test suite to verify your implementation:

```bash
make
./test
```

or

```bash
make run
```

The tests will provide various string formats and verify that your calculation matches the expected architectural behavior of an x86_64 processor.

---

## Files You'll Modify

* **`lib.c`**: Implement the parsing and calculation logic for `lea()`.

## Files Provided

* **`lib.h`**: Register definitions and function prototypes.
* **`test.c`**: Test cases for various address combinations.
* **`Makefile`**: Build instructions.
