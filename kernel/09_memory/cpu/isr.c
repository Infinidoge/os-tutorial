#include "isr.h"
#include "../drivers/keyboard.h"
#include "../drivers/screen.h"
#include "../libc/string.h"
#include "idt.h"
#include "ports.h"
#include "timer.h"

isr_t interrupt_handlers[256];

#define PIC1 0x20
#define PIC2 0xA0
#define COMMAND 0
#define DATA 1

// https://wiki.osdev.org/8259_PIC
void init_pic() {

    // Initialize PIC
    port_byte_out(PIC1 | COMMAND, 0x11);
    port_byte_out(PIC2 | COMMAND, 0x11);

    // Remap the PIC
    port_byte_out(PIC1 | DATA, 0x20); // PIC1 starts at 0x20 (CPU ends at 0x1F)
    port_byte_out(PIC2 | DATA, 0x28); // PIC2 start at 0x28

    port_byte_out(PIC1 | DATA, 0x04); // Tell PIC1 that PIC2 exits at IRQ2 (0000 0100)
    port_byte_out(PIC2 | DATA, 0x02); // Tell PIC2 its cascade identity (0000 0010)

    // Set PIC to 8086/88 mode
    port_byte_out(PIC1 | DATA, 0x01);
    port_byte_out(PIC2 | DATA, 0x01);

    // Clear PIC masks
    port_byte_out(PIC1 | DATA, 0x0);
    port_byte_out(PIC2 | DATA, 0x0);
}

#define ISR_GATE(n) set_idt_gate(n, (size_t)isr##n)
#define IRQ_GATE(n, i) set_idt_gate(n, (size_t)irq##i)

/* Can't do this with a loop because we need the address
 * of the function names */
void isr_install() {
    ISR_GATE(0);
    ISR_GATE(1);
    ISR_GATE(2);
    ISR_GATE(3);
    ISR_GATE(4);
    ISR_GATE(5);
    ISR_GATE(6);
    ISR_GATE(7);
    ISR_GATE(8);
    ISR_GATE(9);
    ISR_GATE(10);
    ISR_GATE(11);
    ISR_GATE(12);
    ISR_GATE(13);
    ISR_GATE(14);
    ISR_GATE(15);
    ISR_GATE(16);
    ISR_GATE(17);
    ISR_GATE(18);
    ISR_GATE(19);
    ISR_GATE(20);
    ISR_GATE(21);
    ISR_GATE(22);
    ISR_GATE(23);
    ISR_GATE(24);
    ISR_GATE(25);
    ISR_GATE(26);
    ISR_GATE(27);
    ISR_GATE(28);
    ISR_GATE(29);
    ISR_GATE(30);
    ISR_GATE(31);

    init_pic();

    // Install the IRQs
    IRQ_GATE(32, 0);
    IRQ_GATE(33, 1);
    IRQ_GATE(34, 2);
    IRQ_GATE(35, 3);
    IRQ_GATE(36, 4);
    IRQ_GATE(37, 5);
    IRQ_GATE(38, 6);
    IRQ_GATE(39, 7);
    IRQ_GATE(40, 8);
    IRQ_GATE(41, 9);
    IRQ_GATE(42, 10);
    IRQ_GATE(43, 11);
    IRQ_GATE(44, 12);
    IRQ_GATE(45, 13);
    IRQ_GATE(46, 14);
    IRQ_GATE(47, 15);

    set_idt(); // Load with ASM
}

/* To print the message which defines every exception */
char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};

void isr_handler(registers_t r) {
    char s[3];
    int_to_ascii(r.int_no, s);
    kprintf("received interrupt: {}\n{}\n", s, exception_messages[r.int_no]);
    asm volatile("hlt");
}

void register_interrupt_handler(uint8_t n, isr_t handler) {
    interrupt_handlers[n] = handler;
}

void irq_handler(registers_t r) {
    /* After every interrupt we need to send an EOI to the PICs
     * or they will not send another interrupt again */
    if (r.int_no >= 40)
        port_byte_out(0xA0, 0x20); /* slave */
    port_byte_out(0x20, 0x20);     /* master */

    /* Handle the interrupt in a more modular way */
    if (interrupt_handlers[r.int_no] != 0) {
        isr_t handler = interrupt_handlers[r.int_no];
        handler(r);
    }
}

void irq_install() {
    /* Enable interruptions */
    asm volatile("sti");
    /* IRQ0: timer */
    init_timer(1000);
    /* IRQ1: keyboard */
    init_keyboard();
}
