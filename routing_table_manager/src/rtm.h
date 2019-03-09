//
// Created by Alexandra Zaharia on 13/02/19.
//

#ifndef LINUX_IPC_RTM_H
#define LINUX_IPC_RTM_H

#include "linked_list.h"

typedef enum { // operation code for possible actions to be taken on the routing table manager (RTM)
    CREATE,
    UPDATE,
    DELETE
} OP_CODE;

typedef unsigned short u16;
#define MASK_MIN 0         // minimum value of destination subnet mask
#define MASK_MAX 32        // maximum value of destination subnet mask
#define IP_ADDR_LEN 16     // maximum length of an IPv4 address, including the null character
#define IP_ADDR_GP 4       // number of dot-separated groups in an IPv4 address
#define IP_ADDR_MIN 0      // minimum value of a dot-separated group in an IPv4 address
#define IP_ADDR_MAX 255    // maximum value of a dot-separated group in an IPv4 address
#define OIF_LEN 32         // maximum length of outgoing interface string, including null character

typedef struct _msg_body { // RTM entry to create, update, or delete
    char destination[16];
    u16 mask;
    char gateway_ip[16];
    char oif[OIF_LEN];     // outgoing interface
} msg_body_t;

typedef struct _sync_msg { // operation on RTM
    OP_CODE op_code;
    msg_body_t msg_body;
} sync_msg_t;

typedef LinkedList RoutingTable;

RoutingTable *routing_table_create();
void routing_table_free(RoutingTable*);

msg_body_t *routing_record_create();
sync_msg_t *rtm_operation_create(OP_CODE, msg_body_t *);

int routing_table_insert(RoutingTable *, msg_body_t *);
int routing_table_update(RoutingTable *, msg_body_t *);
int routing_table_delete(RoutingTable *, msg_body_t *);

void routing_table_print(RoutingTable *);
bool routing_table_contains_dst(RoutingTable *, char *);
bool routing_table_contains_dst_subnet(RoutingTable *, char *, u16);

#endif //LINUX_IPC_RTM_H
