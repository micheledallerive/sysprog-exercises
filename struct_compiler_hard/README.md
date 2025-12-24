# Struct Layout Compiler (Hard)

## Goal

Implement a **struct layout compiler** that calculates field offsets and total sizes for C struct definitions, accounting for proper memory alignment rules—just like a real C compiler does!

Implement the `compile()` function in `lib.c` that takes a `field_info` structure describing a C type and computes:
1. The **offset** of each field within struct (and its nested types!)
2. The **total size** of the type

Given a type definition like:
```c
struct {
  char c;      // offset: 0
  int i;       // offset: 4 (aligned to 4 bytes)
  double d;    // offset: 8 (aligned to 8 bytes)
}              // total size: 16 bytes
```

Your code should calculate the offsets and total size, including any necessary padding for alignment.
Conceptually, your output will have to match what the C compiler would produce using `sizeof()` and `offsetof()`.

## The Data Structure

The `field_info` struct represents types recursively:

```c
struct field_info {
  enum type type;           // CHAR, INT, FLOAT, DOUBLE, POINTER, STRUCT, UNION, ARRAY
  union {
    struct struct_info struct_info;  // For structs
    struct union_info union_info;    // For unions
    struct array_info array_info;    // For arrays
  };
  size_t offset;           // YOU calculate this!
};
```

- **Structs** contain a list of fields
- **Unions** contain a list of alternatives
- **Arrays** contain an element type and length

You are expected to fill in the `offset` field for each field in structs/unions/arrays, and compute the total size of the type. The fields of nested types will need to have a local offset (i.e. relative to the start of the nested type), instead of a global one (from the start of the outer struct).

For convenience, `field_info` is used also when not semantically sound, e.g. as the type of an array or as the initial given type: not always it makes sense to have an offset for those. However, this was chosen to make the implementation simpler, keeping the focus on the main challenge: calculating offsets and sizes.
**Bonus**: how would you redesign the structs to make it more semantically correct?

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

40 tests - 40 pass, 0 fail, 0 skipped
```

You can also add custom logic during testing by modifying the `custom_tests.c` file. Your custom tests will be run after the provided tests.

## Examples

1. Simple struct:
   ```c
   struct {
     char c;      // offset: 0
     int i;       // offset: 4
   }              // total size: 8
   ```

2. Nested struct:
    ```c
    struct {
      char c;           // offset: 0
      struct {          // inner alignment: 8 (due to double)
        char c2;        // offset: 0 (aligned to 1)
        double d;       // offset: 8 (aligned to 8)
      } s;              // inner size: 16
      int i;            // offset: 24 (aligned to 4)
    }                   // total size: 32 (multiple of 8)
    ```

3. Nested union (with array):
    ```c
    struct {
      char c;              // offset: 0
                           // padding: 7 bytes
      union {              // union alignment: 8 (due to double)
        char c2;           // offset: 0 (within union)
        int i2;            // offset: 0 (within union)
        double d;          // offset: 0 (within union)
      } u;                 // union size: 8
      int arr[3];          // offset: 16 (aligned to 4)
                           // size: 12
                           // padding: 4 bytes
    }                      // total size: 32 (multiple of 8)
    ```

## Mistakes to avoid

1. **Forgetting alignment vs size**: A struct with size 8 might only need 4-byte alignment.
   ```c
   struct { char c; float f; }  // size: 8, alignment: 4
   ```

2. **Missing trailing padding**: Struct size must be multiple of its alignment
   ```c
   struct { double d; char c; }  // size: 16, not 9!
   ```

## Files You'll Modify

- **`lib.c`**: Implement `compile()` and helper functions

## Files Provided

- **`test.c`**: 40 comprehensive tests (don't modify)
- **`greatest.h`**: Testing framework (don't modify)
- **`lib.h`**: Data structure definitions (don't modify)
- **`Makefile`**: Build system (ready to use)

