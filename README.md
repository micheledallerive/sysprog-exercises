# Systems Programming Exercise Directory

This repository contains a collection of systems programming exercises designed to reinforce concepts such as memory management, data structures, low-level programming, and compiler design. Each exercise is self-contained within its own directory, complete with implementation files, test suites, and documentation.

---

## Exercise Directory

| Exercise | Description | Difficulty | Exam Relevance\* |
| :--- | :--- | :--- | :--- |
| [**Permission checker**](./permission_checker) | Validate permission of virtual memory accesses. | 🟢 **Easy** | 9/10 |
| [**Struct Compiler (Easy)**](./struct_compiler_easy) | Compute offset and size for simple structs. | 🟢 **Easy** | 9/10 |
| [**LEA**](./lea) | Simulate the LEA x86 instruction. | 🟢 **Easy** | 7/10 |
| [**Vector**](./vector) | Implementation of a dynamic array (vector) with resizing logic. | 🟡 **Medium** | 6/10 |
| [**Linked List**](./linked_list) | Basic pointer manipulation and node management for linear structures. | 🟡 **Medium** | 6/10 |
| [**Backtrace**](./backtrace) | Manual x86_64 stack unwinding using frame pointers and debug symbols. | 🟡 **Medium** | 8/10 |
| [**MLPQ scheduler**](./mlpq_scheduler) | Efficient multi-level priority queue scheduler with O(1) operations. | 🟡 **Medium** | 7/10 |
| [**Bloom Filter**](./bloom_filter) | Probabilistic data structure for set membership testing. | 🟡 **Medium** | 7/10 |
| [**Job Scheduler**](./job_scheduler) | Order jobs based on dependencies using topological sorting. | 🟡 **Medium** | 4/10 |
| [**Struct Compiler (Hard)**](./struct_compiler_hard) | Compile complex structs with nested types and alignment. | 🔴 **Hard** | 8/10 |
| [**Slab Allocator**](./slab_allocator) | Efficient fixed-size memory management with slab allocator | 🔴 **Hard** | 2/10 |
| [**Merkle Tree**](./merkle_tree) | Construct and verify Merkle trees over arbitrary buffers. | 🔴 **Hard** | 5/10 |

\* Exam Relevance does NOT refer to how likely the exercise is to appear on an exam, but rather how important the underlying concepts are for exam preparation.

---

## Project Structure

Each directory follows a standardized layout for ease of use:
* `lib.h`: The public interface and function prototypes you must implement.
* `lib.c`: Your implementation file.
* `test.c`: The automated test suite using the **Greatest** framework.
* `Makefile`: Build automation for the specific exercise.
* `README.md`: Specific goals, requirements, and logic explanations.

## Quick Start

To work on an exercise, navigate to its directory, complete the exercise in `lib.c` and use `make` to compile the tests:

```bash
cd vector
# edit lib.c to implement the required functionality
make run
```

---

### ⚠️ Disclaimer
These exercises are intended purely for **personal practice and self-study**. They do not constitute, represent, or mimic official exam-like exercises. The structure and requirements may differ significantly from actual examination materials.

### 🤖 AI Slop Notice
Please note that **intensive generative AI** was utilized in the creation of the documentation (READMEs) and the testing suites (test files) within this repository to ensure comprehensive coverage and clarity.
