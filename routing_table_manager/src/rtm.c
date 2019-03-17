//
// Created by Alexandra Zaharia on 15/02/19.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "input.h"
#include "rtm.h"
#include "utils.h"
#include "fd_set_mgmt.h"

#define MALLOC_ERROR(func, msg) fprintf(stderr, "%s: %s\n", (func), (msg)) // TODO move elsewhere

int monitored_fd_set[MAX_CLIENTS];
int connection_socket;


// Creates a new routing table.
RoutingTable* routing_table_create()
{
    return linked_list_create();
}


// Frees a routing table
void routing_table_free(RoutingTable *rt)
{
    for (DNode *node = rt->head; node; node = node->next) {
        msg_body_t *record = node->data;
        if (record) free(record);
    }
    rt->free(rt);
}


// Creates a new routing record.
msg_body_t *routing_record_create()
{
    msg_body_t *record = malloc(sizeof(msg_body_t));
    if (!record) {
        MALLOC_ERROR(__func__, "Insufficient memory for routing record");
        return NULL;
    }
    return record;
}


// Inserts a new routing record into the routing table. Returns 0 on success and -1 on failure.
int routing_table_insert(RoutingTable *rt, msg_body_t *record)
{
    msg_body_t *actual_record = routing_record_create();
    if (!actual_record) {
        routing_table_free(rt);
        exit(EXIT_FAILURE);
    }

    strncpy(actual_record->destination, record->destination, IP_ADDR_LEN);
    actual_record->mask = record->mask;
    strncpy(actual_record->gateway_ip, record->gateway_ip, IP_ADDR_LEN);
    strncpy(actual_record->oif, record->oif, OIF_LEN);

    return rt->insert_end(rt, actual_record);
}


/*
 * Updates a routing record with a new gateway IP and a new outgoing interface. Returns 0 on success
 * and -1 on failure.
 */
int routing_table_update(RoutingTable *rt, msg_body_t *record)
{
    if (!routing_table_contains_dst_subnet(rt, record->destination, record->mask)) return -1;

    for (DNode *node = rt->head; node; node = node->next) {
        msg_body_t *current = node->data;
        if (!strcmp(current->destination, record->destination) && current->mask == record->mask) {
            strncpy(current->gateway_ip, record->gateway_ip, IP_ADDR_LEN);
            strncpy(current->oif, record->oif, OIF_LEN);
            return 0;
        }
    }

    return -1;
}


// Deletes a routing record. Returns 0 on success and -1 on failure.
int routing_table_delete(RoutingTable *rt, msg_body_t *record)
{
    int index = -1; // index of record to delete from the routing table
    msg_body_t *current = NULL;

    for (DNode *node = rt->head; node; node = node->next) {
        ++index;
        current = node->data;
        if (!strcmp(current->destination, record->destination) && current->mask == record->mask)
            break;
    }

    if (index == -1) return -1; // record not found
    if (rt->remove_at(rt, index) == NULL) return -1;

    free(current);

    return 0;
}


// Server sends a synchronization message to a client
void send_synchronization_message(const OP_CODE op_code, const msg_body_t record)
{
    sync_msg_t sync_msg;
    memset(&sync_msg, 0, sizeof(sync_msg_t));
    sync_msg.op_code = op_code;
    sync_msg.msg_body = record;

    for (int i = 0; i < MAX_CLIENTS; i++)
        if (monitored_fd_set[i] > 0 && monitored_fd_set[i] != connection_socket)
            if (write(monitored_fd_set[i], &sync_msg, sizeof(sync_msg_t)) == -1) {
                char disconnect_msg[128];
                snprintf(disconnect_msg, 128,
                        "Cannot send synchronization message to client via socket ID %d "
                        "(client disconnected?).", monitored_fd_set[i]);
                error_message(disconnect_msg);
                remove_from_monitored_fd_set(monitored_fd_set[i]);
            }
}

// Displays all records in the routing table to stdout.
void routing_table_print(RoutingTable *rt)
{
    if (rt->size == 0) {
        printf("The routing table has no entries.\n");
        return;
    }

    printf("The routing table has the following ");
    if (rt->size == 1)
        printf("entry:\n");
    else
        printf("%u entries:\n", rt->size);

    char *header_subnet = "Destination subnet";
    char *header_gateway = "   Gateway IP   ";
    char *header_oif = "Outgoing interface";
    const int HDR_SIZE = 64;
    char header[HDR_SIZE];
    snprintf(header, HDR_SIZE, "%s  |  %s  |  %s", header_subnet, header_gateway, header_oif);
    printf("\t%s\n\t", header);
    for (int i = 0; i < strlen(header); i++)
        printf("-");
    printf("\n");

    for (DNode *node = rt->head; node; node = node->next) {
        msg_body_t *record = node->data;
        char dst_subnet[IP_ADDR_LEN + 3];
        snprintf(dst_subnet, IP_ADDR_LEN + 3, "%s/%hu", record->destination, record->mask);
        printf("\t%*s  |  %*s  |  %*s\n",
                (int) strlen(header_subnet), dst_subnet,
                (int) strlen(header_gateway), record->gateway_ip,
                (int) strlen(header_oif), record->oif);
    }
    printf("\n");
}


// Checks whether the given destination IP exists in the routing table.
bool routing_table_contains_dst(RoutingTable *rt, char *destination)
{
    for (DNode *node = rt->head; node; node = node->next) {
        msg_body_t *record = node->data;
        if (strcmp(destination, record->destination) == 0)
            return true;
    }
    return false;
}


// Checks whether the given destination IP and subnet mask exist in the routing table.
bool routing_table_contains_dst_subnet(RoutingTable *rt, char *destination, u16 mask)
{
    for (DNode *node = rt->head; node; node = node->next) {
        msg_body_t *record = node->data;
        if (strcmp(destination, record->destination) == 0 && mask == record->mask)
            return true;
    }
    return false;
}
