/* This will force us to create a kernel entry function instead of jumping to
 * kernel.c:0x00 */
void dummy_test_entrypoint() {}
// use _start instead of main, kinda like begin for arduino
void _start() {
    char *video_memory = (char *)0xb8000;
    *video_memory = 'X';
    unsigned char a;
    unsigned char b;
    a = 0;
    b = 0;
    for (int i = 1; i < 24; ++i) {
        for (int j = 0; j < 80; ++j) {
            *(video_memory + i * 160 + j * 2) = '.';
        }
    }

    *video_memory = 'Z';
    for (int i = 10; i < 20; ++i) {
        for (int j = 50; j < 60; ++j) {
            *(video_memory + i * 160 + j * 2) = 'X';
        }
    }
    while (1) {
        *(video_memory + 0) = b;
        *(video_memory + 1) = ++a;
        b = b + a / 255;
    }
}
