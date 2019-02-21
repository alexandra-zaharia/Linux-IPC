//
// Created by Alexandra Zaharia on 16/02/19.
//

#ifndef LINUX_IPC_INPUT_H
#define LINUX_IPC_INPUT_H

#include "rtm.h"

#define IP_ADDR_LEN 16  // maximum length of an IPv4 address, including the null character
#define IP_ADDR_GP 4    // number of dot-separated groups in an IPv4 address
#define IP_ADDR_MIN 0   // minimum value of a dot-separated group in an IPv4 address
#define IP_ADDR_MAX 255 // maximum value of a dot-separated group in an IPv4 address

int read_ip_address_from_stdin(char *ip_address);
int read_ip_address_from_buffer(char *ip_address);
int read_destination_subnet_from_stdin(char *dst_subnet, char *ip_address, u16 *mask);
int read_destination_subnet_from_buffer(char *dst_subnet, char *ip_address, u16 *mask);


void create_record();

#endif //LINUX_IPC_INPUT_H
