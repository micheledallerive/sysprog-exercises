#ifndef LIB_H
#define LIB_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef uint32_t job_id_t;

#define MAX_PRIORITY 64
#define HIGHEST_PRIORITY (MAX_PRIORITY - 1)
#define LOWEST_PRIORITY 0

typedef struct job {
  job_id_t id;
  uint8_t priority;

  struct job *next;
  struct job *prev;
} job_t;

typedef struct {
  job_t *head;
  job_t *tail;
} job_queue_t;

typedef struct {
  job_queue_t queues[MAX_PRIORITY];
  uint64_t ready_bitset;
} scheduler_t;

void scheduler_init(scheduler_t *scheduler);
/**
 * Add a job to the scheduler's ready queue based on its priority.
 */
void scheduler_add_job(scheduler_t *scheduler, job_t *job);
/**
 * Retrieve the next job to run based on priority.
 * Returns NULL if no jobs are ready.
 */
job_t *scheduler_next(scheduler_t *scheduler);
/**
 * Remove a job from the scheduler's ready queue.
 */
void scheduler_remove_job(scheduler_t *scheduler, job_t *job);

#endif  // LIB_H
