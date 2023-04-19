#include "timer.h"
#include "../libc/function.h"
#include "isr.h"
#include "ports.h"
#include "types.h"

volatile uint32_t tick = 0;

volatile uint32_t get_tick() {
    return tick;
}

void wait(uint32_t ticks) {
    uint32_t current = tick;
    while (tick <= current + ticks) {
        asm volatile("nop");
    }
}

static void timer_callback(registers_t regs) {
    UNUSED(regs);
    tick++;
}

void init_timer(uint32_t freq) {
    /* Install the function we just wrote */
    register_interrupt_handler(IRQ0, timer_callback);

    /* Get the PIT value: hardware clock at 1193180 Hz */
    uint32_t divisor = 1193180 / freq;
    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);
    /* Send the command */
    port_byte_out(0x43, 0x36); /* Command port */
    port_byte_out(0x40, low);
    port_byte_out(0x40, high);
}
