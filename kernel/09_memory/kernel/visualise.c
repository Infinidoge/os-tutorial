#include "visualise.h"
#include "../cpu/timer.h"
#include "../cpu/types.h"
#include "../drivers/keyboard.h"
#include "../drivers/screen.h"
#include "../libc/mem.h"
#include "shell.h"

#define SPEED_FACTOR 75

#define ARRAY_SIZE 20
#define BAR_WIDTH (MAX_COLS / ARRAY_SIZE)

#define ARRAY_COLOR 0x50 // Purple/Magenta

#define ARRAY_STARTING_ROW 1
#define STATUS_ROW 0
#define LINE1 21
#define LINE2 22
#define LINE3 23
#define LINE4 24

void render_array(const int *array, const int row) {
    paint_rect(' ', 0, 0, row, MAX_COLS, row + 19, true);
    for (int i = 0; i < ARRAY_SIZE; i++) {
        paint_rect(' ', ARRAY_COLOR, BAR_WIDTH * i, row, BAR_WIDTH, array[i], true);
    }
}

enum Colors {
    NONE = 0,        // Black
    SELECTED = 0x40, // Red
    SPECIAL = 0x30,  // Cyan
    DONE = 0x20,     // Green
    INDEX = 0x60,    // Yellow
};

#define mark_position(position, color) paint_rect(' ', color, ((position) * (BAR_WIDTH)), 0, BAR_WIDTH, 1, true)

void render_status(const enum Colors array[]) {
    for (int i = 0; i < ARRAY_SIZE; i++) {
        mark_position(i, array[i]);
    }
}

void mark_many(enum Colors *array, int lower, int upper, enum Colors status) {
    for (int i = lower; i < upper; i++) {
        array[i] = status;
    }
}

void clear_info() {
    paint_rect(' ', 0, 0, 21, MAX_COLS, 4, true);
}

void clear_line(int row) {
    paint_rect(' ', 0, 0, row, MAX_COLS, 1, true);
}

int rand() {
    static int number = 1;
    number = (number + (get_tick() * (get_tick() / 2) * (get_tick() / 4) * (get_tick() / 8))) % RAND_MAX;
    return number;
}

void swap(int *array, int first, int second) {
    int t = array[second];
    array[second] = array[first];
    array[first] = t;
}

static volatile bool running = true;

#define kprintf_at(col, row)                 \
    set_cursor_offset(get_offset(col, row)); \
    kprintf

void visualise_bubble(int *array) {
    int current = 0;
    int max = ARRAY_SIZE - 1;
    bool swapped = false;

    enum Colors status[ARRAY_SIZE];
    mark_many(status, 0, ARRAY_SIZE, NONE);

    while (running) {
        mark_many(status, current, current + 2, SELECTED);

        render_status(status);

        mark_many(status, current, current + 2, NONE);

        clear_info();

        kprintf_at(0, LINE1)("Checking positions {i} and {i}", current, current + 1);

        if (array[current] > array[current + 1]) {
            swap(array, current, current + 1);
            swapped = true;
            kprintf_at(0, LINE2)("Swapping");
        }

        wait(5 * SPEED_FACTOR);
        render_array(array, ARRAY_STARTING_ROW);

        if (++current >= max) {
            if (swapped == false) {
                mark_many(status, 0, ARRAY_SIZE, DONE);
                clear_info();
                kprintf_at(0, LINE1)("No swaps occurred in last sweep, done!");
                render_status(status);
                return;
            } else {
                swapped = false;
                status[max] = DONE;
                kprintf_at(0, LINE3)("{i} is in its final place", current + 1);
                current = 0;
                max--;
                wait(10 * SPEED_FACTOR);
                render_status(status);
            }
        }

        wait(5 * SPEED_FACTOR);
    }
}

void visualise_insertion(int *array) {
    int current = 0;
    int max = ARRAY_SIZE - 1;
    bool swapped = false;

    enum Colors status[ARRAY_SIZE];
    mark_many(status, 0, ARRAY_SIZE, NONE);
    render_status(status);

    for (int i = 0; running && i < ARRAY_SIZE; i++) {
        clear_info();
        kprintf_at(0, LINE1)("Working from position {i}", i);

        for (int j = i; running && j > 0; j--) {
            clear_info();
            kprintf_at(0, LINE1)("Working from position {i}", i);
            status[i] = SPECIAL;
            status[j] = SELECTED;
            status[j - 1] = SELECTED;
            render_status(status);
            status[j] = NONE;
            status[j - 1] = NONE;

            kprintf_at(0, LINE2)("Comparing positions {i} and {i}", j - 1, j);

            wait(5 * SPEED_FACTOR);
            if (array[j - 1] > array[j]) {
                kprintf_at(0, LINE3)("Swapping");
                swap(array, j, j - 1);
                wait(5 * SPEED_FACTOR);
                render_array(array, ARRAY_STARTING_ROW);
            } else {
                kprintf_at(0, LINE3)("Finished inserting element, continuing");
                render_status(status);
                wait(5 * SPEED_FACTOR);
                break;
            }
        }

        status[i] = NONE;

        wait(5 * SPEED_FACTOR);
    }

    mark_many(status, 0, ARRAY_SIZE, DONE);
    render_status(status);
    clear_info();
    kprintf_at(0, LINE1)("Done!");
}

#define RETURN_IF(variable) \
    if (variable)           \
    return

#define RETURN_VALUE_IF(variable, value) \
    if (variable)                        \
    return value

int partition(int *array, int low, int high, enum Colors *status) {
    int pivot = array[high];
    kprintf_at(0, LINE2)("Pivot is {i}", pivot);
    status[high] = SPECIAL;
    render_status(status);
    wait(5 * SPEED_FACTOR);
    clear_line(LINE2);

    int i = low - 1;
    if (status[i] == NONE) {
        status[i] = INDEX;
        render_status(status);
    }

    for (int j = low; j < high; j++) {
        RETURN_VALUE_IF(!running, 0);
        kprintf_at(0, LINE2)("Comparing {i} with pivot", j);
        status[j] = SELECTED;
        wait(5 * SPEED_FACTOR);
        RETURN_VALUE_IF(!running, 0);
        render_status(status);
        if (array[j] <= pivot) {
            RETURN_VALUE_IF(!running, 0);
            if (status[i] == INDEX)
                status[i] = NONE;
            i++;
            if (status[i] == NONE)
                status[i] = INDEX;
            render_status(status);
            kprintf_at(0, LINE3)("Swapping");
            wait(5 * SPEED_FACTOR);
            RETURN_VALUE_IF(!running, 0);
            swap(array, i, j);
            render_array(array, ARRAY_STARTING_ROW);
        }
        clear_line(LINE2);
        clear_line(LINE3);
        status[j] = NONE;
    }

    if (status[i] == INDEX)
        status[i] = NONE;
    i++;
    kprintf_at(0, LINE2)("Moving pivot element into correct position ({i})", i);
    RETURN_VALUE_IF(!running, 0);
    wait(5 * SPEED_FACTOR);
    RETURN_VALUE_IF(!running, 0);
    swap(array, i, high);
    status[high] = NONE;
    status[i] = SPECIAL;
    render_array(array, ARRAY_STARTING_ROW);
    return i;
}

void quicksort(int *array, int low, int high, enum Colors *status) {
    if (low == high)
        status[low] = DONE;
    if (low >= high || low < 0)
        return;

    clear_info();
    kprintf_at(0, LINE1)("Partitioning array from {i} to {i}", low, high);

    int p = partition(array, low, high, status);
    RETURN_IF(!running);
    status[p] = DONE;
    render_status(status);
    render_array(array, ARRAY_STARTING_ROW);

    clear_info();
    kprintf_at(0, LINE1)("Running quicksort from {i} to {i}", low, p - 1);
    wait(5 * SPEED_FACTOR);
    RETURN_IF(!running);
    quicksort(array, low, p - 1, status);
    RETURN_IF(!running);
    render_array(array, ARRAY_STARTING_ROW);

    clear_info();
    kprintf_at(0, LINE1)("Running quicksort from {i} to {i}", p + 1, high);
    wait(5 * SPEED_FACTOR);
    RETURN_IF(!running);
    quicksort(array, p + 1, high, status);
    RETURN_IF(!running);
    render_array(array, ARRAY_STARTING_ROW);
}

void visualise_quick(int *array) {
    enum Colors status[ARRAY_SIZE];
    mark_many(status, 0, ARRAY_SIZE, NONE);
    render_status(status);

    RETURN_IF(!running);
    quicksort(array, 0, ARRAY_SIZE - 1, status);
    RETURN_IF(!running);

    mark_many(status, 0, ARRAY_SIZE, DONE);
    clear_info();
    kprintf_at(0, LINE1)("Done!");
    render_status(status);
}

void merge(int *array, int low, int middle, int high) {
    int low2 = middle + 1;

    if (array[middle] <= array[low2])
        return;

    while (low <= middle && low2 <= high) {
        RETURN_IF(!running);
        if (array[low] <= array[low2]) {
            low++;
        } else {
            int value = array[low2];

            for (int i = low2; i > low; i--) {
                array[i] = array[i - 1];
            }
            array[low] = value;

            low++;
            middle++;
            low2++;
        }
        render_array(array, ARRAY_STARTING_ROW);
        wait(3 * SPEED_FACTOR);
    }
}

void mergesort(enum Colors *status, int *array, int low, int high) {
    if (low >= high)
        return;

    int middle = (low + high) / 2;

    RETURN_IF(!running);
    if (middle - low > 1) {
        clear_info();
        kprintf_at(0, LINE1)("Running mergesort from {i} to {i}", low, middle);
        wait(5 * SPEED_FACTOR);
    }

    RETURN_IF(!running);
    mergesort(status, array, low, middle);

    RETURN_IF(!running);
    if (high - middle - 1 > 1) {
        clear_info();
        kprintf_at(0, LINE1)("Running mergesort from {i} to {i}", middle + 1, high);
        wait(5 * SPEED_FACTOR);
    }

    RETURN_IF(!running);
    mergesort(status, array, middle + 1, high);

    RETURN_IF(!running);
    clear_info();
    kprintf_at(0, LINE1)("Merging {i} to {i} and {i} to {i}", low, middle, middle + 1, high);
    mark_many(status, low, middle + 1, SELECTED);
    mark_many(status, middle + 1, high + 1, SPECIAL);
    render_status(status);
    wait(5 * SPEED_FACTOR);
    mark_many(status, low, high + 1, NONE);
    RETURN_IF(!running);
    merge(array, low, middle, high);
    RETURN_IF(!running);
}

void visualise_merge(int *array) {
    enum Colors status[ARRAY_SIZE];
    mark_many(status, 0, ARRAY_SIZE, NONE);
    render_status(status);

    clear_info();
    kprintf_at(0, LINE1)("Running mergesort");
    wait(5 * SPEED_FACTOR);
    RETURN_IF(!running);
    mergesort(status, array, 0, ARRAY_SIZE - 1);
    RETURN_IF(!running);
    clear_info();
    kprintf_at(0, LINE1)("Done!");
    mark_many(status, 0, ARRAY_SIZE, DONE);
    render_status(status);
    render_array(array, ARRAY_STARTING_ROW);
}

void visualise_key_handler(uint8_t scancode) {
    if (scancode == 16) {
        running = false;
    }
}

enum SortingAlgorithms algorithm = INSERTION;

void visualiser() {
    static screenstate *prev_screen = NULL;
    if (prev_screen == NULL)
        prev_screen = (screenstate *)kmalloc_naive(sizeof(screenstate), false, NULL);

    save_screen_to(prev_screen);
    clear_screen();
    set_cursor_offset(get_offset(0, 23));

    keyhandler previous_handler = swap_key_handler(&visualise_key_handler);

    int array[ARRAY_SIZE];

    // Fill array
    for (int i = 0; i < ARRAY_SIZE; i++) {
        array[i] = i + 1;
    }

    // Shuffle array
    for (int i = 0; i < ARRAY_SIZE - 1; i++) {
        int j = i + rand() / (RAND_MAX / (ARRAY_SIZE - i) + 1);
        int t = array[j];
        array[j] = array[i];
        array[i] = t;
    }

    render_array(array, ARRAY_STARTING_ROW);
    clear_info();
    kprintf_at(0, LINE1)("Starting array");
    wait(20 * SPEED_FACTOR);

    switch (algorithm) {
    case BUBBLE:
        visualise_bubble(array);
        break;
    case INSERTION:
        visualise_insertion(array);
        break;
    case QUICK:
        visualise_quick(array);
        break;
    case MERGE:
        visualise_merge(array);
        break;
    }

    while (running) {
        asm volatile("nop");
    }

    running = true;
    load_screen_from(prev_screen);
    return_key_handler(previous_handler);
}
