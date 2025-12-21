# Stack Backtrace Unwinder

## Goal

Implement a **stack backtrace unwinder** that manually traverses the call stack to identify the sequence of function calls. This exercise focuses on understanding the x86_64 stack frame layout and using debug information to map instruction addresses back to source code locations.

Implement the `print_backtrace()` function in `lib.c` that takes a pointer to the current frame pointer (RBP), an array of debugging information, and the entry count. Your goal is to follow the chain of frame pointers to identify the chain of function calls, and print each function's source file and name along with the return address.

For example, if the stack contains a call chain where `main` called `funcA`, which then called `funcB`, the output should look like this:

```text
0x400300 file3.c:funcB
0x400200 file2.c:funcA
0x400100 file1.c:main
```

The format for each line must be: `<return_address> <file_name>:<function_name>`.

Note that the stack that you will traverse is a simulated stack: you shouldn't go through the actual stack of the running program, but rather start from the given rbp and follow the x86_64 calling conventions to unwind the stack.

---

## The Data Structure

The `debugging_info` struct maps memory address ranges to their respective source code metadata:

```c
struct debugging_info {
  const char *file_name;
  const char *function_name;
  void *start_addr;
  void *end_addr;
};
```

---

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

The tests use the **Greatest** testing framework to simulate various stack configurations in memory and verify that your unwinder correctly identifies every caller in the chain.

You should see:

```text
* Suite stack_traversal_suite:
..........

10 tests - 10 pass, 0 fail, 0 skipped
```

---

## Files You'll Modify

* **`lib.c`**: Implement `print_backtrace()` and your address-to-symbol lookup logic.

## Files Provided

* **`lib.h`**: Struct definitions and function prototypes.
* **`test.c`**: Testing suite that mocks stack frames for verification.
* **`Makefile`**: Build instructions.
