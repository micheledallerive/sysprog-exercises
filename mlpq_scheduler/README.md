# Multilevel Priority Queue (MLPQ) Scheduler

## Goal

Implement a **Multilevel Priority Queue (MLPQ) Scheduler**, a core component of many operating systems (including Linux and various RTOSs). This exercise focuses on low-level data structure management, pointer arithmetic, and architectural optimization using bitmasks to achieve **$O(1)$** complexity for scheduling operations.

You must implement the library functions defined in `lib.h` to manage task readiness and selection across multiple priority levels.

---

## The Problem

A scheduler's job is to decide which task (or "job") should run next. In a priority-based system, the highest-priority task that is ready to run must always be selected first. If multiple tasks share the same priority, they should be handled in a Round-Robin fashion.

To make this efficient enough for an OS kernel, we cannot simply loop through all tasks. Instead, we use:
1.  **An array of queues**: Each index corresponds to a priority level ($0$ to $63$).
2.  **A Bitmask**: A single 64-bit integer (`uint64_t`) where each bit represents whether a specific priority queue has tasks in it.

### Data Structures

* **`job_t`**: A structure representing a single process/task. It is an element in a **doubly linked list**.
* **`job_queue_t`**: A simple head/tail wrapper for the doubly linked list at each priority level.
* **`scheduler_t`**: The main controller containing the array of 64 queues and the `ready_bitset` used for fast lookups.



---

## Constraints and Requirements

* **$O(1)$ Complexity**: The `scheduler_next` and `scheduler_add_job` functions must run in constant time. You must not use loops to find the highest-priority task.
* **Bit Manipulation**: Use the `__builtin_clzll` (Count Leading Zeros) compiler intrinsic to find the highest set bit in your 64-bit bitset instantly.
* **Doubly Linked List**: You must manually manage `next` and `prev` pointers. When a job is removed or popped, ensure its pointers are set to `NULL` to prevent "dangling" references.
* **FIFO Ordering**: Within a single priority level, the scheduler must return tasks in the order they were added.

---

## Example Scenario

1.  **Add Job A** (Priority 10). `ready_bitset` becomes `0...010000000000` (bit 10 is set).
2.  **Add Job B** (Priority 20). `ready_bitset` becomes `0...100000000010000000000` (bits 10 and 20 are set).
3.  **Call `scheduler_next`**: 
    * The system identifies bit 20 as the highest set bit.
    * Job B is popped from the Priority 20 queue.
    * Bit 20 is cleared in the bitset because the queue is now empty.
    * Job B is returned.
4.  **Call `scheduler_next`**:
    * The system identifies bit 10 as the highest set bit.
    * Job A is popped and returned.

---

## Testing Your Code

The provided test suite includes 30 test cases covering:
* **Priority Preemption**: Ensuring a priority 63 job always beats a priority 0 job.
* **FIFO Validation**: Ensuring same-priority jobs follow Round-Robin order.
* **Bitset Synchronization**: Verifying the bitmask is updated correctly when queues become empty.
* **Pointer Integrity**: Ensuring doubly linked list operations (add/remove) don't corrupt the heap or lose track of the tail.
* **Boundary Cases**: Testing the lowest (0) and highest (63) possible priorities.

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
* Suite mlpq_suite:
..............................

30 tests - 30 pass, 0 fail, 0 skipped
```

---

## Files You'll Modify

* **`lib.c`**: Implement the scheduler initialization, job addition, job removal, and next-job selection logic.

## Files Provided

* **`lib.h`**: Header containing the data structures and function prototypes.
* **`greatest.h`**: The unit testing framework.
* **`Makefile`**: Build instructions.
