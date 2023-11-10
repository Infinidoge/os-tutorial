#ifndef LINKEDLIST_
#define LINKEDLIST_

typedef struct Node {
    void *data;
    struct Node *prev;
    struct Node *next;
} node;

#define NEXT(node) node = node->next
#define PREV(node) node = node->prev

#endif
