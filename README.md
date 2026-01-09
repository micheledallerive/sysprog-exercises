# Systems Programming Exercise Directory

This repository contains a collection of systems programming exercises designed to practice C programming and reinforce concepts like memory management, data structures, low-level programming, and similar. Each exercise is self-contained within its own directory, complete with implementation files, test suites, and documentation.

---

## Exercise Directory

| Exercise | Description | Difficulty | Relevant topics \* |
| :--- | :--- | :--- | :--- |
| [**Endianness Swapper**](./endianness) | Convert integers between Little and Big Endian formats. | 🟢 **Easy** | Bitwise operations, Memory layout |
| [**fpclassify**](./fpclassify) | Classify floating-point numbers into categories (normal, subnormal, zero, infinite, NaN). | 🟢 **Easy** | Floating-point numbers, Bitwise operations |
| [**Permission checker**](./permission_checker) | Validate permission of virtual memory accesses. | 🟢 **Easy** | MMU, Virtual memory, Bitwise operations |
| [**Struct Compiler (Easy)**](./struct_compiler_easy) | Compute offset and size for simple structs. | 🟢 **Easy** | Memory layout |
| [**Static Stack**](./staticstack) | Implement a generic fixed-capacity stack data structure. | 🟢 **Easy** | Memory management, Implementation |
| [**LEB128**](./leb128) | Compress and decompress integers using LEB128 encoding. | 🟢 **Easy** | Bitwise operations, Compression |
| [**LEA**](./lea) | Simulate the LEA x86 instruction. | 🟢 **Easy** | x86 assembly |
| [**Vector**](./vector) | Implementation of a dynamic array (vector) with resizing logic. | 🟡 **Medium** | Memory management, Implementation |
| [**Linked List**](./linked_list) | Basic pointer manipulation and node management for linear structures. | 🟡 **Medium** | Pointers, Implementation |
| [**Hash Map**](./hashmap) | Hash map with collision resolution using separate chaining. | 🟡 **Medium** | Pointers, Hashing, Implementation |
| [**Backtrace**](./backtrace) | Manual x86_64 stack unwinding using frame pointers and debug symbols. | 🟡 **Medium** | x86 assembly, Calling conventions |
| [**MLPQ scheduler**](./mlpq_scheduler) | Efficient multi-level priority queue scheduler with O(1) operations. | 🟡 **Medium** | Bitwise operations, Implementation |
| [**Bloom Filter**](./bloom_filter) | Probabilistic data structure for set membership testing. | 🟡 **Medium** | Bitwise operations, Implementation |
| [**Job Scheduler**](./job_scheduler) | Order jobs based on dependencies using topological sorting. | 🟡 **Medium** | Implementation |
| [**Struct Compiler (Hard)**](./struct_compiler_hard) | Compile complex structs with nested types and alignment. | 🔴 **Hard** | Memory layout, Implementation |
| [**Slab Allocator**](./slab) | Efficient fixed-size memory management with slab allocator | 🔴 **Hard** | Memory management, Implementation |
| [**Merkle Tree**](./merkle_tree) | Construct and verify Merkle trees over arbitrary buffers. | 🔴 **Hard** | Hashing, Implementation |
| [**Firewall**](./firewall) | Network packet filtering based on different rules | 🟣 **Very Hard** | Networking, Data Structures, Implementation |

\* Topic "Implementation" refers to exercise which heavily focus on building data structures or algorithms from scratch, rather than solely testing theoretical knowledge.

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

### Disclaimer
These exercises are intended purely for **personal practice and self-study**. They do not constitute, represent, or mimic official exam-like exercises. The structure and requirements may differ significantly from actual examination materials.

### Contributing
If you encounter any issues with an exercise, find any lack of clarity, or have ideas for new exercises, please feel free to open an issue in this repository or reach out to me directly via my ETH email (mdalle). Your feedback and contributions are highly appreciated!

### AI Slop Notice
Please note that **generative AI** was utilized in the creation of the READMEs and the testing suites within this repository, which may result in occasional inaccuracies or inconsistencies.

