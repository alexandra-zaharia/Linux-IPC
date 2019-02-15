//
// Created by Alexandra Zaharia on 15/02/19.
//

#include "rtm.h"


// Creates a new routing table.
RoutingTable* routing_table_create()
{
    return linked_list_create();
}


// Frees a routing table
void routing_table_free(RoutingTable* rt)
{
    rt->free(rt);
}

// Inserts a new routing record into the routing table. Returns 0 on success and -1 on failure.
int routing_table_insert(RoutingTable* rt, msg_body_t* record)
{
    return 0;
}

/*
 * Updates a routing record with a new gateway IP and a new outgoing interface. Returns 0 on success
 * and -1 on failure.
 */
int routing_table_update(RoutingTable* rt, char* dst, u16 mask, char* gw_ip, char* oif)
{
    return 0;
}

// Deletes a routing record. Returns 0 on success and -1 on failure.
int routing_table_delete(RoutingTable* rt, char* dst, u16 mask)
{
    return 0;
}

// Displays all records in the routing table to stdout.
void routing_table_print(RoutingTable* rt)
{

}
