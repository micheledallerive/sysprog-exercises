# Struct Layout Compiler (Easy)

## Goal

Implement a **struct layout compiler** that calculates field offsets and total sizes for simple C struct definitions. This exercise focuses on the fundamental logic of memory alignment and padding used by C compilers.

Implement the `compile()` function in `lib.c` that takes an array of `struct field` objects and computes:
1. The **offset** of each field within the struct.
2. The **total size** of the struct.

Given a struct definition like:
```c
struct {
  char c;      // offset: 0
  int i;       // offset: 4 (aligned to 4 bytes)
  double d;    // offset: 8 (aligned to 8 bytes)
}              // size: 16 bytes
```

Your code should calculate these offsets based on the natural alignment requirements of each type.

## The Data Structure

The `field` struct represents the components of your struct:

```c
struct field {
  enum type type;  // FIELD_TYPE_CHAR, FIELD_TYPE_INT, FIELD_TYPE_FLOAT, etc.
  size_t offset;   // YOU calculate this!
};
```

Unlike the [advanced version](../struct_compiler_hard/README.md), this exercise is **flat**. You do not need to handle nested structs, unions, or arrays.

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

The tests use the **Greatest** testing framework and verify your implementation against real C compiler behavior using `sizeof()` and `offsetof()`.

You should see:
```
* Suite struct_layout_tests:
........................................

20 tests - 20 pass, 0 fail, 0 skipped
```

You can also add custom logic during testing by modifying the `custom_tests.c` file. Your custom tests will be run after the provided tests.

## Examples

1. **Basic Alignment**:
   ```c
   struct {
     char c;      // offset: 0
     int i;       // offset: 4 (3 bytes of padding after 'c')
   }              // size: 8
   ```

2. **Mixed Types**:
   ```c
   struct {
     int i;       // offset: 0
     char c;      // offset: 4
     double d;    // offset: 8 (3 bytes of padding after 'c')
   }              // size: 16
   ```

## Files You'll Modify

* **`lib.c`**: Implement `compile()` and your alignment helper functions.

## Files Provided

* **`lib.h`**: Enum and struct definitions.
* **`test.c`**: Your testing suite using `greatest.h`.
* **`Makefile`**: Build instructions.
