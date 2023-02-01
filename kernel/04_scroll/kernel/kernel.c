#include "../drivers/screen.h"
#include "util.h"

void _start() {
    clear_screen();

    /* Fill up the screen */
    int i = 0;
    for (i = 0; i < 30; i++) {
        char str[255];
        int_to_ascii(i, str);
        kprint_at(str, 0, i);
        for (int j = 0; j < 100000000; j++) {
            ;
        }
    }

    kprint_at("This text forces the kernel to scroll. Row 0 will disappear. ",
        60, 24);
    kprint("And with this text, the kernel will scroll again, and row 1 will "
           "disappear too!");
}
