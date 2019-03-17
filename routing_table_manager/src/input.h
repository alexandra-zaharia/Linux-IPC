//
// Created by Alexandra Zaharia on 16/02/19.
//

#ifndef LINUX_IPC_INPUT_H
#define LINUX_IPC_INPUT_H

#include "rtm.h"

typedef enum { // type of operation the admin inputs to the server console
    IDLE,      // no operation in process, i.e. the admin menu should be shown
    CREATING,
    UPDATING,
    DELETING
} INPUT_STATE;

typedef enum { // type of entry the admin is in the process of introducing to the routing table
    SUBNET,    // admin is in the process of entering a subnet (create, update, delete)
    GATEWAY,   // admin is in the process of entering a gateway IP (create, update)
    OIF        // admin is in the process of entering an outgoing interface (create, update)
} ENTRY_TYPE;

int read_ip_address_from_stdin(char *ip_address);
int read_ip_address_from_buffer(char *ip_address);
int read_destination_subnet_from_stdin(char *dst_subnet, char *ip_address, u16 *mask);
int read_destination_subnet_from_buffer(char *dst_subnet, char *ip_address, u16 *mask);

void create_record(RoutingTable *rtm, char *buffer, INPUT_STATE *state, ENTRY_TYPE *entry,
                   msg_body_t *record);
void update_record(RoutingTable *rtm, char *buffer, INPUT_STATE *state, ENTRY_TYPE *entry,
                   msg_body_t *record);
void delete_record(RoutingTable *rtm, char *buffer, INPUT_STATE *state, msg_body_t *record);

void show_routing_menu(RoutingTable *rtm);

#endif //LINUX_IPC_INPUT_H
