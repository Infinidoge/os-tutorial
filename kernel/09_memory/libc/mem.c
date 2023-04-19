#include "mem.h"
#include "../drivers/screen.h"
#include "meta.h"
#include "string.h"

////////// Utilities //////////

void memory_copy(uint8_t *source, uint8_t *dest, size_t nbytes) {
    for (int i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}

void memory_set(uint8_t *dest, uint8_t val, uint32_t len) {
    uint8_t *temp = (uint8_t *)dest;
    for (; len != 0; len--)
        *temp++ = val;
}

////////// Naive Allocator //////////

size_t kmalloc_naive(size_t size, bool align, size_t *phys_addr) {
    static size_t free_mem_addr = END;

    if (align && (free_mem_addr & 0xFFF)) {
        free_mem_addr &= 0xFFFFF000;
        free_mem_addr += 0x1000;
    }

    if (phys_addr)
        *phys_addr = free_mem_addr;

    size_t ret = free_mem_addr;
    free_mem_addr += size;
    return ret;
}

///////// Linked List Implementation //////////

#define NEXT(node) node = node->next
#define PREV(node) node = node->prev

node *create_node(size_t address, size_t size) {
    node *new_node = (node *)kmalloc_naive(sizeof(node), false, NULL);
    new_node->address = address;
    new_node->size = size;
    new_node->prev = new_node->next = NULL;

    return new_node;
}

node *insert_after(node *head, node *new) {
    if (head->next != NULL) {
        new->next = head->next;
        new->next->prev = new;
    }

    head->next = new;
    new->prev = head;

    return head;
}

node *insert_before(node *head, node *new) {
    if (head->prev != NULL) {
        new->prev = head->prev;
        new->prev->next = new;
    }

    head->prev = new;
    new->next = head;

    return new;
}

node *find(node *list, size_t address) {
    node *current = list;

    while (current != NULL && current->address != address) {
        current = current->next;
    }

    return current;
}

node *head(node *list) {
    if (list == NULL)
        return NULL;

    node *current = list;
    while (current->prev != NULL)
        current = current->prev;

    return current;
}

node *tail(node *list) {
    if (list == NULL)
        return NULL;

    node *current = list;
    while (current->next != NULL)
        current = current->next;

    return current;
}

node *delete_by_address(node *list, size_t address) {
    node *target = find(list, address);

    if (target == NULL)
        return list;

    if (target->prev != NULL)
        target->prev->next = target->next;

    if (target->next != NULL)
        target->next->prev = target->prev;

    if (target->prev == NULL)
        return target->next;
    else
        return list;
}

node *insert_by_address(node *sorted, node *new_node) {
    if (sorted == NULL) {
        new_node->prev = new_node->next = NULL;
        return new_node;
    } else if (sorted->address >= new_node->address) {
        return insert_before(sorted, new_node);
    } else {
        node *current = sorted;
        while (current->next != NULL && current->next->address < new_node->address) {
            current = current->next;
        }

        insert_after(current, new_node);

        return sorted;
    }
}

node *sort_by_address(node *list) {
    // Implemented via insertion sort
    node *sorted = NULL;
    node *current = list;
    node *next = NULL;

    while (current != NULL) {
        next = current->next;

        sorted = insert_by_address(sorted, current);

        current = next;
    }

    return sorted;
}

#define IFASCENDING(left, op, right) ((ascending && left op right) || right op left)

node *insert_by_size(node *sorted, node *new_node, bool ascending) {
    if (sorted == NULL) {
        new_node->prev = new_node->next = NULL;
        return new_node;
    } else if IFASCENDING (sorted->size, >=, new_node->size) {
        return insert_before(sorted, new_node);
    } else {
        node *current = sorted;
        while (current->next != NULL && IFASCENDING(current->next->size, <, new_node->size)) {
            current = current->next;
        }

        insert_after(current, new_node);

        return sorted;
    }
}

#undef IFASCENDING

node *sort_by_size(node *list, bool ascending) {
    // Implemented via insertion sort
    node *sorted = NULL;
    node *current = list;
    node *next = NULL;

    while (current != NULL) {
        next = current->next;

        sorted = insert_by_size(sorted, current, ascending);

        current = next;
    }

    return sorted;
}

node *add_new(node *list, size_t address, size_t size) {
    return insert_by_address(list, create_node(address, size));
}

void resize(node *list, size_t address, size_t size) {
    node *target = find(list, address);

    if (target != NULL)
        target->size = size;
}

size_t length(node *list) {
    int length = 0;

    node *current = list;
    while (current != NULL) {
        length++;
        current = current->next;
    }

    return length;
}

node *clone_list(node *list) {
    node *new_list;

    node *current = list;
    while (current != NULL) {
        new_list = add_new(new_list, current->address, current->size);
        current = current->next;
    }

    return new_list;
}

///////// alloc implementation //////////

static node *allocated = NULL;
static node *free = NULL;

const size_t FREE_MEM_START = 0x10000;
const size_t FREE_MEM_END = 0x40000;

void init_memory() {
    free = create_node(FREE_MEM_START, FREE_MEM_END - FREE_MEM_START);
}

size_t kmalloc(size_t size, bool align, size_t align_size, enum FitType fit) {
    if (align && (size % align_size != 0)) {
        size += align_size - (size % align_size);
    }

    node *free_target;
    node *current;

    switch (fit) {
    case FIRST:
        free_target = free;
        break;

    case BEST:
        free = sort_by_size(free, true);

        current = free;
        while (current != NULL && current->size < size) {
            current = current->next;
        }

        if (current == NULL)
            return (size_t)NULL;

        free_target = current;
        break;

    case WORST:
        free = sort_by_size(free, false);

        if (free->size < size)
            return (size_t)NULL;

        free_target = free;
        break;
    }

    size_t address = free_target->address;

    // Allocate the memory
    allocated = add_new(allocated, address, size);

    if (free_target->size == size)
        free = delete_by_address(free, address);
    else {
        free_target->address += size;
        free_target->size -= size;
    }

    free = sort_by_address(free);

    return address;
}

size_t kcalloc(size_t n, size_t size);

size_t krealloc(size_t address, size_t size, bool align, size_t align_size);

void merge_free() {}

void kfree(size_t address) {
    node *target = find(allocated, address);
    if (target == NULL)
        return;

    allocated = delete_by_address(allocated, address);
    free = add_new(free, address, target->size);

    // Merge free together; Unfinished
    /* node *current = free; */
    /* while (current->next != NULL) { */
    /*     if ((current->address + current->size) == current->next->address) { */
    /*         current->size += current->next->size; */
    /*         current = delete_by_address(current, current->next->address); */
    /*     } */

    /*     current = current->next; */
    /* } */

    /* node *head = current; */
    /* while (head->prev != NULL) { */
    /*     head = head->prev; */
    /* } */

    /* free = head; */
}

void print_memory() {
    kprintlnf("Total Physical Memory: {i}kb", (FREE_MEM_END - FREE_MEM_START) / 1024);

    int free_total = 0;
    node *current = free;
    while (current != NULL) {
        free_total += current->size;
        current = current->next;
    }
    kprintlnf("Total Free: {i}kb", free_total / 1024);

    int allocated_total = 0;
    current = allocated;
    while (current != NULL) {
        allocated_total += current->size;
        current = current->next;
    }
    kprintlnf("Total Allocated: {i}kb", allocated_total / 1024);

    kprintlnf("Number of allocations: {i}", length(allocated));
    kprintlnf("Number of free gaps: {i}", length(free));
    kprintlnf("Start of Memory: {i}", FREE_MEM_START);
    kprintlnf("End of Memory: {i}", FREE_MEM_END - 1);
}

void memory_map() {
    kprintln("Memory Map:");

    node *current_allocated = allocated;
    node *current_free = free;

    while (current_allocated != NULL || current_free != NULL) {
        if (current_free == NULL || current_allocated->address < current_free->address) {
            kprintlnf("{x} - {x} Allocated region of {i}kb", current_allocated->address,
                current_allocated->address + current_allocated->size - 1, current_allocated->size / 1024);
            NEXT(current_allocated);
        } else if (current_allocated == NULL || current_free->address < current_allocated->address) {

            kprintlnf("{x} - {x} Free region of {i}kb", current_free->address,
                current_free->address + current_free->size - 1, current_free->size / 1024);
            NEXT(current_free);
        }
    }
}
