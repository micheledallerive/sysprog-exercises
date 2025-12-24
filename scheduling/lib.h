#ifndef LIB_H
#define LIB_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef uint32_t job_id_t;

typedef struct {
  job_id_t job_id;
  job_id_t depends_on_id;
} job_dependency_t;

/**
 * Sort the jobs with id in range [0, num_jobs) based on the given dependencies.
 * Each dependency indicates that 'job_id' depends on 'depends_on_id', meaning
 * that 'depends_on_id' must be completed before 'job_id' can start.
 * If a valid scheduling order exists, the function returns a dynamically
 * allocated array of job IDs in the order they should be executed.
 * The caller is responsible for freeing this array. If no valid order exists
 * (due to cycles in dependencies), the function returns NULL.
 */
job_id_t *schedule_jobs(size_t num_jobs, size_t num_dependencies,
                        const job_dependency_t *dependencies);

#endif  // LIB_H
