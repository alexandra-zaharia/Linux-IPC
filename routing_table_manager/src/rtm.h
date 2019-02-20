//
// Created by Alexandra Zaharia on 13/02/19.
//

#ifndef LINUX_IPC_RTM_H
#define LINUX_IPC_RTM_H

#include "linked_list.h"
//#include "input.h"

typedef enum { // operation code for possible actions to be taken on the routing table manager (RTM)
    CREATE,
    UPDATE,
    DELETE
} OP_CODE;

typedef unsigned short u16;
#define MASK_MIN 0  // minimum value of destination subnet mask
#define MASK_MAX 32 // maximum value of destination subnet mask

#define OIF_LEN 32  // maximum length of outgoing interface string, including null character

typedef struct _msg_body { // RTM entry to create, update, or delete
    char destination[16];
    u16 mask;
    char gateway_ip[16];
    char oif[OIF_LEN];
} msg_body_t;

typedef struct _sync_msg { // operation on RTM
    OP_CODE op_code;
    msg_body_t msg_body;
} sync_msg_t;

typedef LinkedList RoutingTable;

RoutingTable* routing_table_create();
void routing_table_free(RoutingTable*);

int routing_table_insert(RoutingTable*, msg_body_t*);
int routing_table_update(RoutingTable*, char*, u16, char*, char*);
int routing_table_delete(RoutingTable*, char*, u16);
void routing_table_print(RoutingTable*);

#endif //LINUX_IPC_RTM_H
