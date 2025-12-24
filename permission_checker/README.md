# Virtual Memory Address Translator

## Goal

Implement a **virtual-to-physical address translator** that simulates the behavior of a Memory Management Unit (MMU). This exercise focuses on bitwise manipulation, page table walking, and memory protection enforcement.

You must implement the `access_memory()` function in `lib.c`. This function takes a pointer to a **single-level** page table, a virtual address, an intended action (Read, Write, or Execute), and a pointer to store the resulting physical address.

---

## Address Format

The simulated architecture uses a 32-bit address space, but only the lower 22 bits are used for translation:

* **Virtual Page Number (VPN):** 14 bits
* **Page Offset:** 8 bits
* **Total Virtual Address Width:** $14 + 8 = 22$ bits. The upper 10 bits must be ignored.

---

## The Data Structure

### Page Table Entry (PTE)
A `page_table_entry_t` is a 16-bit value with the following layout:

| Bits | Description |
| :--- | :--- |
| **0-11** | Physical Page Number (PPN) |
| **12** | Valid Bit (1 = Valid, 0 = Fault) |
| **13** | Read Permission (1 = Allowed) |
| **14** | Write Permission (1 = Allowed) |
| **15** | Execute Permission (1 = Allowed) |

### Page Table
The page table is a simple array of entries indexed by the VPN:

```c
struct page_table {
    page_table_entry_t entries[1 << VPN_BITS];
};
```

---

## Return Codes

The function returns a `result_t` enum:
* `SUCCESS`: Translation successful, `physical_address` is populated.
* `PAGE_FAULT`: The Valid bit is 0 or the virtual address exceeds the architectural limit.
* `PERMISSION_DENIED`: The requested action (READ, WRITE, or EXECUTE) is not permitted by the PTE bits.

---

## Testing Your Code

You can verify your implementation against the provided test cases:

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
* Suite permchecker_suite:
....................

19 tests - 19 pass, 0 fail, 0 skipped
```

---

## Files You'll Modify

* **`lib.c`**: Implement `access_memory()` and any necessary bit-masking logic.

## Files Provided

* **`lib.h`**: Constant definitions (VPN_BITS, PPN_BITS, etc.) and function prototypes.
* **`greatest.h`**: The unit testing framework.
* **`Makefile`**: Build instructions.
