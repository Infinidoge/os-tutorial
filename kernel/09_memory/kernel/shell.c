#include "shell.h"
#include "../drivers/keyboard.h"
#include "../drivers/screen.h"
#include "../libc/function.h"
#include "../libc/mem.h"
#include "../libc/string.h"
/* #include "program.h" */
#include "scheduler.h"
#include "visualise.h"

typedef struct Command {
    const char *key;
    void (*func)(const char *input);
    const char *help;
} command;

#define CMD(name) void cmd_##name(const char *input)
#define CMDREF(name, help) \
    { #name, cmd_##name, help }

CMD(end);
CMD(test);
/* CMD(program); */
CMD(visualise);
CMD(memory);
CMD(memory_info);
CMD(memory_map);
CMD(colors);
CMD(help);
CMD(echo);
CMD(clear);

const command commands[] = {
    CMDREF(end, "Halts the CPU"),
    CMDREF(test, "Runs whatever test code is currently in place"),
    /* CMDREF(program, "Runs the program"), */
    CMDREF(visualise, "Runs the visualiser"),
    CMDREF(memory, "Prints out the current status and a map of main memory"),
    CMDREF(memory_info, "Prints out the current status of main memory"),
    CMDREF(memory_map, "Prints out a map of main memory"),
    CMDREF(colors, "Prints out all of the colors, with color codes"),
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
}

/* CMD(program) { */
/*     UNUSED(input); */
/*     schedule(&program); */
/* } */

CMD(visualise) {
    if (strcmp(input, "bubble") == 0)
        algorithm = BUBBLE;
    else if (strcmp(input, "insertion") == 0)
        algorithm = INSERTION;
    else if (strcmp(input, "quick") == 0)
        algorithm = QUICK;
    else if (strcmp(input, "merge") == 0)
        algorithm = MERGE;
    else
        algorithm = BUBBLE;

    schedule(&visualiser);
}

CMD(memory) {
    UNUSED(input);

    print_memory();
    memory_map();
}

CMD(memory_info) {
    UNUSED(input);
    print_memory();
}

CMD(memory_map) {
    UNUSED(input);
    memory_map();
}

CMD(colors) {
    for (int i = 0; i < 16; i++) {
        paint((char)i + 48, i, i, 0);
    }
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

static char key_buffer[256];

static void user_input() {
    bool found = false;
    for (int i = 0; i < LEN(commands); i++) {
        int last;
        const command *cmd = &commands[i];
        if (strbeginswith(key_buffer, cmd->key, &last)) {
            if (key_buffer[last] == ' ') {
                cmd->func(&key_buffer[last + 1]);
                found = true;
            } else if (key_buffer[last] == '\0') {
                cmd->func("");
                found = true;
            }
        }
    }

    key_buffer[0] = '\0';

    if (!found)
        kprintln("Invalid command.");

    schedule(&print_prompt);
}

static void shell_key_handler(uint8_t scancode) {
    static bool next_upper = false;

    bool KEY_BUFFER_EMPTY = key_buffer[0] == '\0';

    if (scancode == BACKSPACE) {
        if (!KEY_BUFFER_EMPTY) {
            backspace(key_buffer);
            kprint_backspace();
        }
    } else if (scancode == ENTER) {
        kprint("\n");
        schedule(&user_input);
    } else if (scancode == LSHIFT || scancode == RSHIFT) {
        next_upper = true;
    } else {
        char letter = get_letter(scancode, next_upper);
        if (next_upper)
            next_upper = false;

        /* Remember that kprint only accepts char[] */
        char str[2] = {letter, '\0'};
        append(key_buffer, letter);
        kprint(str);
    }
}

void init_shell() {
    kprintln("Type something, it will go through the kernel");
    kprintln("Type help for a list of commands");
    kprintln("Type end to halt the CPU");

    print_prompt();

    key_handler = &shell_key_handler;
}
