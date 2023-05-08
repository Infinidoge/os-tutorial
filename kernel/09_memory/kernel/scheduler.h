#ifndef SCHEDULER_H_
#define SCHEDULER_H_

typedef void (*schedulable)();

void schedule(schedulable);

void run_scheduler();
void stop();

#endif // SCHEDULER_H_
