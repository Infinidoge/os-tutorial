#include "../cpu/idt.h"
#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "util.h"

void irq_demonstration(registers_t r) {
    kprint("Called from an IRQ demonstration function!");
}

void _start() {
    isr_install();
    /* Test the interrupts */
    register_interrupt_handler(IRQ0, irq_demonstration);

    asm volatile("int $32");
}
