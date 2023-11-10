#include "linkedlist.h"
#include "../cpu/types.h"

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
