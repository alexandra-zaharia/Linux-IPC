//
// Created by Alexandra Zaharia on 16/02/19.
//

#ifndef LINUX_IPC_INPUT_H
#define LINUX_IPC_INPUT_H

#include "rtm.h"

int read_ip_address_from_stdin(char *ip_address);
int read_ip_address_from_buffer(char *ip_address);
int read_destination_subnet_from_stdin(char *dst_subnet, char *ip_address, u16 *mask);
int read_destination_subnet_from_buffer(char *dst_subnet, char *ip_address, u16 *mask);

sync_msg_t create_record(RoutingTable *rtm);
sync_msg_t update_record(RoutingTable *rtm);
sync_msg_t delete_record(RoutingTable *rtm);

#endif //LINUX_IPC_INPUT_H
