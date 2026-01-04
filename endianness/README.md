# Endianness Swapper

## Goal

Implement a library to handle **endianness conversion**.

You must implement functions to detect the host machine's endianness at runtime and to manually swap the byte order of 16-bit, 32-bit, and 64-bit integers.

---

## Constraints and Requirements

* **No Built-ins**: You are strictly forbidden from using standard library functions like `htons`, `htonl`, `ntohl`, or compiler built-ins like `__builtin_bswap32`. You must implement the logic manually using bitwise operations.
* **Runtime Detection**: Your `get_system_endianness` function must determine the byte order at runtime by inspecting memory, not by using pre-processor macros like `__BYTE_ORDER__`.

---

## Core API

### Detection
* **`get_system_endianness`**: Returns `LITTLE_ENDIAN` or `BIG_ENDIAN` based on the host system's byte order. If the endianness cannot be determined, it should return `OTHER_ENDIAN`.

### Conversion
* **`swap_uint16`**: Swaps bytes for a 16-bit value (e.g., `0xAABB` -> `0xBBAA`).
* **`swap_uint32`**: Swaps bytes for a 32-bit value.
* **`swap_uint64`**: Swaps bytes for a 64-bit value.

---

## Testing Your Code

The provided test suite verifies:
* Host system endianness detection.
* Bitwise symmetry.
* Correct byte ordering.

To run the tests:

```bash
make
./test
```

or

```bash
make run
```

You should see:

```text
* Suite detection_suite:
.
* Suite conversion_suite:
.....

6 tests - 6 pass, 0 fail, 0 skipped
```

---

## Files You'll Modify

* **`lib.c`**: Implement the detection and swapping logic.

## Files Provided

* **`lib.h`**: Enum definitions and function prototypes.
* **`test.c`**: The unit testing framework.
* **`Makefile`**: Build instructions.
