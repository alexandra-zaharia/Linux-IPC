//
// Created by Alexandra Zaharia on 18/12/18.
//

#ifndef LIBGCDS_NODE_H
#define LIBGCDS_NODE_H

// Simply linked node: a node data structure with a pointer to the next element
typedef struct Node Node;
typedef Node Item;

struct Node {
    void* data;
    Node* next;
};

Node* node_create(void*);
Item* item_create(void*);

// Doubly linked node: a node data structure with a pointer to the next and previous elements
typedef struct DNode DNode;

struct DNode {
    void* data;
    DNode* next;
    DNode* prev;
};

DNode* dnode_create(void*);

#endif //LIBGCDS_NODE_H
