#include "scheduler.h"
#include "../cpu/types.h"
#include "../drivers/screen.h"

schedulable scheduled[256];

static uint8_t current, running = 0;

void schedule(schedulable program) {
    /* kprintlnf("Scheduling {i}", current); */
    scheduled[current++] = program;
}

void run_scheduler() {
    while (true) {
        if (running != current && scheduled[running] != NULL) {
            /* kprintlnf("Running {i}", running); */
            (*scheduled[running++])();
        }
    }
}
