#include "../cpu/idt.h"
#include "../cpu/isr.h"
#include "../drivers/screen.h"
#include "util.h"

void _start() {
    isr_install();
    /* Test the interrupts */
    asm volatile("int $2");
    asm volatile("int $3");
}
