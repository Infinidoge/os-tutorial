#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "../libc/function.h"

typedef action0 schedulable;

void schedule(schedulable);

void run_scheduler();
void stop();

#endif // SCHEDULER_H_
