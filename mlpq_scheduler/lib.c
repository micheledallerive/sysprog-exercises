#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"

void scheduler_init(scheduler_t *scheduler) {}

void scheduler_add_job(scheduler_t *scheduler, job_t *job) {}

job_t *scheduler_next(scheduler_t *scheduler) {
  return NULL;
}

void scheduler_remove_job(scheduler_t *scheduler, job_t *job) {}
