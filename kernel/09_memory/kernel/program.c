#include "program.h"
#include "../cpu/timer.h"
#include "../cpu/types.h"
#include "../drivers/keyboard.h"
#include "../drivers/screen.h"
#include "../libc/mem.h"

static volatile bool running = true;

void program_key_handler(uint8_t scancode) {
    if (scancode == 16) {
        running = false;
    }
}

void program() {
    static screenstate *prev_screen = NULL;
    if (prev_screen == NULL)
        prev_screen = (screenstate *)kmalloc_naive(sizeof(screenstate), false, NULL);

    save_screen_to(prev_screen);
    clear_screen();

    keyhandler previous_handler = swap_key_handler(&program_key_handler);

    int col = 0;
    int color = 0;
    while (running) {
        paint(' ', color << 4, col, 0);
        if (++col == MAX_COLS) {
            col = 0;
            if (++color == 0x10)
                color = 0;
        }
        wait(20);
    }

    running = true;
    load_screen_from(prev_screen);
    return_key_handler(previous_handler);
}
