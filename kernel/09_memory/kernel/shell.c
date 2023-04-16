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
CMD(page);
CMD(help);
CMD(echo);
CMD(clear);

const command commands[] = {
    CMDREF(end, "Halts the CPU"),
    CMDREF(page, "Allocates a page of memory and prints the address to it"),
    CMDREF(help, "Prints a list of commands with help text"),
    CMDREF(echo, "Echos the input back to you"),
    CMDREF(clear, "Clears the screen"),
};

CMD(end) {
    UNUSED(input);
    kprint("Stopping the CPU. Bye!\n");
    asm volatile("hlt");
}

CMD(page) {
    UNUSED(input);
    size_t phys_addr;
    size_t page = kmalloc_naive(1000, true, &phys_addr);

    char page_str[16];
    hex_to_ascii(page, page_str);
    char phys_str[16];
    hex_to_ascii(phys_addr, phys_str);

    kprintlnf("Page: {}, Physical Address: {}", page_str, phys_str);
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
