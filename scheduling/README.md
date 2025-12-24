# Job Scheduler (Topological Sort)

## Goal

Implement a **job scheduler** that determines a valid execution order for a set of tasks with interdependencies. This exercise focuses on graph theory, specifically directed acyclic graphs (DAGs) and topological sorting algorithms.

You must implement the `schedule_jobs()` function in `lib.c`. This function takes the total number of jobs and a list of dependencies, returning an array of job IDs in an order that ensures every dependency is met before the dependent job runs.

---

## The Problem

You are given $N$ jobs, identified by IDs from $0$ to $N-1$. Some jobs depend on others. For example, if Job 1 depends on Job 0, Job 0 must appear earlier in the resulting schedule than Job 1.

### Data Structures

The dependencies are provided as an array of `job_dependency_t`:

- `job_id`: The ID of the job that has a dependency.
- `depends_on_id`: The ID of the job that must be completed first.

### Function Signature

```c
job_id_t *schedule_jobs(size_t num_jobs, size_t num_dependencies, const job_dependency_t *dependencies);
```

---

## Constraints and Requirements

* **Valid Schedule:** A schedule is valid if for every dependency `(A, B)`, job `B` appears before job `A` in the output array.
* **Cycles:** If the dependencies contain a cycle (e.g., A depends on B, and B depends on A), a valid schedule is impossible. In this case, the function must return `NULL`.
* **Memory Management:** The function must return a pointer to a heap-allocated array of `job_id_t` containing all `num_jobs` IDs. The caller is responsible for freeing this memory.
* **Disconnected Graphs:** If some jobs have no dependencies or belong to separate groups, they can be scheduled in any order relative to each other, as long as their internal dependencies are respected.

---

## Testing Your Code

The provided test suite includes 20 test cases covering:
* Simple linear chains and star topologies.
* Complex Directed Acyclic Graphs (DAGs).
* Cycles (ensuring `NULL` is returned).
* Large independent sets and disconnected components.

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
* Suite dependency_suite:
....................

20 tests - 20 pass, 0 fail, 0 skipped
```

---

## Files You'll Modify

* **`lib.c`**: Implement `schedule_jobs()` and any helper graph structures.

## Files Provided

* **`lib.h`**: Type definitions and function prototypes.
* **`greatest.h`**: The unit testing framework.
* **`Makefile`**: Build instructions.
