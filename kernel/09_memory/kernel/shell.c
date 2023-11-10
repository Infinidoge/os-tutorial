#include "shell.h"
#include "../cpu/info.h"
#include "../cpu/types.h"
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
CMD(cpuid);
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
    CMDREF(cpuid, "Prints out information about the CPU"),
    CMDREF(colors, "Prints out all of the colors, with color codes"),
    CMDREF(help, "Prints a list of commands with help text"),
    CMDREF(echo, "Echos the input back to you"),
    CMDREF(clear, "Clears the screen"),
};

CMD(end) {
    UNUSED(input);
    kprint("Exiting. Bye!\n");
    schedule(&stop);
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

CMD(cpuid) {
    cpuid_registers registers;
    if (strlen(input) == 0 || strcmp(input, "1") == 0) {
        cpu_information information = cpu_info();
        kprintlnf("Model: {u}", information.model);
        kprintlnf("Family ID: {u}", information.family_id);

        kprint("Processor Type: ");
        switch (information.processor_type) {
        case ORIGINAL_OEM:
            kprintln("Original OEM");
            break;
        case INTEL_OVERDRIVE:
            kprintln("Intel Overdrive");
            break;
        case DUAL_PROCESSOR:
            kprintln("Dual Processor");
            break;
        case INTEL_RESERVED:
            kprintln("Intel Reserved");
            break;
        }

        kprintlnf("CLFLUSH Line Size: {u}", information.clflush_line_size);
        kprintlnf("Maximum Logical Processors: {u}", information.maximum_logical_processors);
        kprintlnf("Initial APIC ID: {u}", information.initial_apic_id);

        kprintlnf("APIC: {B}", information.apic);
        kprintlnf("SEP: {B}", information.sep);
        kprintlnf("MMX: {B}", information.mmx);
    } else if (strcmp(input, "0") == 0) {
        registers = cpuid(0, 0);
        kprintlnf("Maximum CPUID input: {x}", registers.eax);

        char string[13];
        string[0] = BYTE(registers.ebx, 0);
        string[1] = BYTE(registers.ebx, 8);
        string[2] = BYTE(registers.ebx, 16);
        string[3] = BYTE(registers.ebx, 24);
        string[4] = BYTE(registers.edx, 0);
        string[5] = BYTE(registers.edx, 8);
        string[6] = BYTE(registers.edx, 16);
        string[7] = BYTE(registers.edx, 24);
        string[8] = BYTE(registers.ecx, 0);
        string[9] = BYTE(registers.ecx, 8);
        string[10] = BYTE(registers.ecx, 16);
        string[11] = BYTE(registers.ecx, 24);
        string[12] = '\0';

        kprintlnf("Manufacturer ID: {}", string);
    }
}

CMD(colors) {
    UNUSED(input);
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
    kprintln("Type end to exit");

    print_prompt();

    key_handler = &shell_key_handler;
}
