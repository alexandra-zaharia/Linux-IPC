//
// Created by Alexandra Zaharia on 13/12/18.
//

#ifndef LIBGCDS_LINKED_LIST_H
#define LIBGCDS_LINKED_LIST_H

#include <stdbool.h>
#include "node.h"

typedef struct LinkedList LinkedList;

struct LinkedList {
    DNode* head;                               // first node of the linked list
    DNode* tail;                               // last node of the linked list
    unsigned int size;                         // number of items in the linked list

    void (*free)(LinkedList*);                 // frees the linked list
    bool (*is_empty)(LinkedList*);             // is the linked list empty?

    int (*insert_start)(LinkedList*, void*);   // adds an item at the front of the linked list
    int (*insert_end)(LinkedList*, void*);     // adds an item at the end of the linked list
    int (*insert_at)(LinkedList*, void*, int); // adds an item at a specified position

    void* (*remove_start)(LinkedList*);        // removes and returns item at front of linked list
    void* (*remove_end)(LinkedList*);          // removes and returns item at end of linked list
    void* (*remove_at)(LinkedList*, int);      // removes and returns item at specified position

    bool (*contains)(LinkedList*, void*);      // does the list contain a given item?
    int (*index)(LinkedList*, void*);          // returns the index of an item in the linked list
};

LinkedList* linked_list_create();

#endif //LIBGCDS_LINKED_LIST_H
