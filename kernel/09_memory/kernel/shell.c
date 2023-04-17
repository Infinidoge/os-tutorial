#include "shell.h"
#include "../drivers/screen.h"
#include "../libc/function.h"
#include "../libc/mem.h"
#include "../libc/string.h"

struct Command {
    const char *key;
    void (*func)(const char *input);
    const char *help;
};

#define CMD(name) void cmd_##name(const char *input)
#define CMDREF(name, help) \
    { #name, cmd_##name, help }

CMD(end);
CMD(test);
CMD(page);
CMD(alloc);
CMD(memory);
CMD(help);
CMD(echo);
CMD(clear);

const command commands[] = {
    CMDREF(end, "Halts the CPU"),
    CMDREF(test, "Runs whatever test code is currently in place"),
    CMDREF(page, "Allocates a page of memory and prints the address to it"),
    CMDREF(alloc, "Allocates a single node of memory"),
    CMDREF(memory, "Prints out the current status of main memory"),
    CMDREF(help, "Prints a list of commands with help text"),
    CMDREF(echo, "Echos the input back to you"),
    CMDREF(clear, "Clears the screen"),
};

CMD(end) {
    UNUSED(input);
    kprint("Stopping the CPU. Bye!\n");
    asm volatile("hlt");
}

CMD(test) {
    UNUSED(input);

    size_t pointer1 = kmalloc(100, true, 1024, FIRST);
    DEBUG_POINTER(pointer1);
    size_t pointer2 = kmalloc(2000, true, 1024, BEST);
    DEBUG_POINTER(pointer2);
    size_t pointer3 = kmalloc(4000, true, 1024, WORST);
    DEBUG_POINTER(pointer3);

    kfree(pointer2);
}

CMD(page) {
    UNUSED(input);
    size_t phys_addr;
    size_t page = kmalloc_naive(0x1000, true, &phys_addr);

    char page_str[16];
    hex_to_ascii(page, page_str);
    char phys_str[16];
    hex_to_ascii(phys_addr, phys_str);

    kprintlnf("Page: {}, Physical Address: {}", page_str, phys_str);
}

CMD(alloc) {
    UNUSED(input);
    static int count = 0;
    static size_t pointer1, pointer2, pointer3, pointer4, pointer5, pointer6, pointer7;

    if (count == 0) {
        pointer1 = kmalloc(100, true, 1024, FIRST);
        pointer2 = kmalloc(100, true, 1024, FIRST);
        pointer3 = kmalloc(100, true, 1024, FIRST);
        pointer4 = kmalloc(100, true, 1024, FIRST);
        pointer5 = kmalloc(100, true, 1024, FIRST);

        print_memory();
    }

    if (count == 1) {
        kfree(pointer1);
        print_memory();
    }

    if (count == 2) {
        kfree(pointer5);
        print_memory();
    }

    if (count == 3) {
        pointer6 = kmalloc(100, true, 1024, FIRST);
        kfree(pointer1 - 100);
        print_memory();
    }

    if (count == 4) {
        kfree(pointer3);
        print_memory();
    }

    if (count == 5) {
        kfree(pointer1);
        kfree(pointer2);
        kfree(pointer3);
        kfree(pointer4);
        kfree(pointer5);
        kfree(pointer6);
        kfree(pointer7);
        kfree(pointer7 + 10000);
        print_memory();
    }

    count++;
}

CMD(memory) {
    UNUSED(input);

    print_memory();
}

CMD(help) {
    UNUSED(input);
    kprint("----- HELP -----\n");
    for (int i = 0; i < LEN(commands); i++) {
        kprintf("{}: {}\n", commands[i].key, commands[i].help);
    }
    kprint("\n");
}

CMD(echo) {
    kprintf("{}\n", input);
}

CMD(clear) {
    clear_screen();
}

void user_input(const char *input) {
    bool found = false;
    for (int i = 0; i < LEN(commands); i++) {
        int last;
        const command *cmd = &commands[i];
        if (strbeginswith(input, cmd->key, &last)) {
            if (input[last] == ' ') {
                cmd->func(&input[last + 1]);
                found = true;
            } else if (input[last] == '\0') {
                cmd->func("");
                found = true;
            }
        }
    }

    if (!found)
        kprintln("Invalid command.");

    print_prompt();
}
