//
// Created by Alexandra Zaharia on 16/02/19.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "rtm.h"
#include "utils.h"
#include "input.h"


/*
 * Reads an IPv4 address from STDIN and stores it into the `ip_address` buffer, in the form
 * xxx.xxx.xxx.xxx. Returns 0 on success and -1 on failure.
 *
 * In order for an IPv4 address to be valid, it needs:
 *     - to contain precisely 3 '.' (dot) symbols
 *     - the 3 dots need to delimit precisely 4 groups of digits
 *     - each group of digits needs to be an integer between 0 and 255
 */
int read_ip_address_from_stdin(char *ip_address)
{
    if (read_line(ip_address, IP_ADDR_LEN) == -1) return -1;
    return read_ip_address_from_buffer(ip_address);
}


/*
 * Reads an IPv4 address from the `ip_address` buffer, in the form xxx.xxx.xxx.xxx. Returns 0 on
 * success and -1 on failure. Valid IPv4 addresses follow the same rules as for
 * `read_ip_address_from_stdin`.
 */
int read_ip_address_from_buffer(char *ip_address)
{
    if (!ip_address || strchr(ip_address, '.') == NULL) return -1;

    /* Create a copy of the input string to work on (`buffer`), since it gets modified by `strsep`.
     * A reference to the beginning of the buffer needs to be kept (`head`) in order to ensure
     * correct de-allocation. */
    char *buffer = strndup(ip_address, IP_ADDR_LEN); // work on a copy of the input string
    char *head = buffer;                             // keep a reference to the head of the buffer
    char *token;
    int gp_count = 0;

    while ((token = strsep(&buffer, ".")) != NULL) {
        int ip_gp;
        if (read_int_from_buffer(token, &ip_gp) == -1) goto clean_fail;
        if (ip_gp < IP_ADDR_MIN || ip_gp > IP_ADDR_MAX) goto clean_fail;
        ++gp_count;
    }
    free(head);                                      // when here, `buffer` is NULL
    head = NULL;
    if (gp_count == IP_ADDR_GP) return 0;

clean_fail:
    if (head) free(head);
    return -1;
}


/*
 * Reads a destination subnet from the `dst_subnet` buffer in the form xxx.xxx.xxx.xxx/yy, where the
 * IPv4 address xxx.xxx.xxx.xxx is stored into the `ip_address` buffer and the subnet mask yy is
 * stored into `mask`. Returns 0 on success and -1 on failure.
 */
int read_destination_subnet_from_buffer(char *dst_subnet, char *ip_address, u16 *mask)
{
    if (strchr(dst_subnet, '/') == NULL) return -1;

    char *token;
    int token_count = 0;

    while ((token = strsep(&dst_subnet, "/")) != NULL) {
        if (++token_count > 2) return -1;
        switch (token_count) {
            case 1: {
                char buffer[IP_ADDR_LEN];
                strncpy(buffer, token, IP_ADDR_LEN);
                if (read_ip_address_from_buffer(buffer) == -1) return -1;
                strncpy(ip_address, token, IP_ADDR_LEN);
            }; break;
            case 2: {
                int _mask;
                if (read_int_from_buffer(token, &_mask) == -1) return -1;
                if (_mask < MASK_MIN || _mask > MASK_MAX) return -1;
                *mask = (u16) _mask;
            }; break;
            default: return -1;
        }
    }

    return 0;
}


/*
 * Reads a destination subnet from STDIN into the `dst_subnet` buffer in the form
 * xxx.xxx.xxx.xxx/yy, where xxx.xxx.xxx.xxx is an IPv4 address that follows the same rules as for
 * `read_ip_address_from_stdin`, and yy is the subnet mask (an integer between 0 and 32). The IPv4
 * address is stored in the `ip_address` buffer, and the subnet mask is stored in the u16 variable
 * pointed to by `mask`. Returns 0 on success and -1 on failure.
 */
int read_destination_subnet_from_stdin(char *dst_subnet, char *ip_address, u16 *mask)
{
    if (read_line(dst_subnet, IP_ADDR_LEN + 3) == -1) return -1;
    char dst_subnet_copy[IP_ADDR_LEN + 3];
    strncpy(dst_subnet_copy, dst_subnet, IP_ADDR_LEN + 3);
    return read_destination_subnet_from_buffer(dst_subnet_copy, ip_address, mask);
}


/*
 * Reads information into and returns a routing table record for a create, update, or delete
 * operation.
 */
msg_body_t *_read_record(RoutingTable *rtm, OP_CODE op_code)
{
    msg_body_t *record = routing_record_create();
    if (!record) {
        routing_table_free(rtm);
        exit(EXIT_FAILURE);
    }

    switch (op_code) {
        case CREATE: printf("Enter a new record:\n"); break;
        case UPDATE: printf("Update an existing record:\n"); break;
        case DELETE: printf("Delete an existing record:\n"); break;
        default: error_message("Unknown operation code");
    }

    // Read destination subnet (all operations)
    char dst_subnet[IP_ADDR_LEN + 3];
    printf("\tEnter destination subnet (xxx.xxx.xxx.xxx/yy): ");
    int status = read_destination_subnet_from_stdin(dst_subnet, record->destination, &record->mask);
    while (status == -1
            || (op_code != CREATE && !routing_table_contains_dst(rtm, record->destination))
            || (op_code != CREATE && !routing_table_contains_dst_subnet(
                    rtm, record->destination, record->mask))) {
        if (status == -1)
            error_message("\tIncorrect destination subnet format. Try again.");
        else
            error_message("\tThe specified record does not exist in the routing table. Try again.");
        printf("\tEnter destination subnet (xxx.xxx.xxx.xxx/yy): ");
        status = read_destination_subnet_from_stdin(dst_subnet, record->destination, &record->mask);
    }

    // Stop here for the delete operation
    if (op_code == DELETE) return record;

    // Read gateway IP (for create and update operations)
    switch (op_code) {
        case CREATE: printf("\tEnter gateway IP (xxx.xxx.xxx.xxx): "); break;
        case UPDATE: printf("\tEnter new gateway IP (xxx.xxx.xxx.xxx): "); break;
        default: error_message("Unknown operation code");
    }
    while (read_ip_address_from_stdin(record->gateway_ip) == -1) {
        error_message("\tIncorrect IP address format. Try again.");
        printf("\tEnter gateway IP (xxx.xxx.xxx.xxx): ");
    }

    // Read outgoing interface (for create and update operations)
    switch (op_code) {
        case CREATE: printf("\tEnter outgoing interface: "); break;
        case UPDATE: printf("\tEnter new outgoing interface: "); break;
        default: error_message("Unknown operation code");
    }
    while (read_line(record->oif, OIF_LEN) == -1) {
        error_message("\tInvalid outgoing interface. Try again.");
        printf("\tEnter outgoing interface: ");
    }

    return record;
}


/*
 * Prompts the administrator to enter a new routing table record and returns the corresponding
 * synchronization message the server needs to send to all connected clients.
 */
sync_msg_t create_record(RoutingTable *rtm)
{
    msg_body_t *record = _read_record(rtm, CREATE);
    printf("Adding record %s/%hu with gateway %s and outgoing interface %s\n",
            record->destination, record->mask, record->gateway_ip, record->oif);
    if (routing_table_insert(rtm, record) == -1)
        error_message("Could not insert record.");
    printf("\n");

    sync_msg_t sync_msg = {CREATE, *record};
    return sync_msg;
}


/*
 * Prompts the administrator to update an existing routing table record and returns the
 * corresponding synchronization message the server needs to send to all connected clients.
 */
sync_msg_t update_record(RoutingTable *rtm)
{
    msg_body_t *record = _read_record(rtm, UPDATE);
    printf("Updating record %s/%hu with gateway %s and outgoing interface %s\n",
           record->destination, record->mask, record->gateway_ip, record->oif);
    if (routing_table_update(rtm, record) == -1)
        error_message("Could not update record.");
    printf("\n");

    sync_msg_t sync_msg = {UPDATE, *record};
    free(record);
    return sync_msg;
}


/*
 * Prompts the administrator to delete an existing routing table record and returns the
 * corresponding synchronization message the server needs to send to all connected clients.
 */
sync_msg_t delete_record(RoutingTable *rtm)
{
    msg_body_t *record = _read_record(rtm, DELETE);
    printf("Deleting record %s/%hu\n", record->destination, record->mask);
    if (routing_table_delete(rtm, record) == -1)
        error_message("Could not delete record.");
    printf("\n");

    sync_msg_t sync_msg = {DELETE, *record};
    free(record);
    return sync_msg;
}