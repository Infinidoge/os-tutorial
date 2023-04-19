#ifndef TIMER_H
#define TIMER_H

#include "types.h"

volatile uint32_t get_tick();
void init_timer(uint32_t freq);

void wait(uint32_t ticks);

#endif
