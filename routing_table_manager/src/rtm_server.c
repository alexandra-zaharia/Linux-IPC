//
// Created by Alexandra Zaharia on 13/02/19.
//

/*
 * The server process maintains a routing table in which a record is defined by an IPv4 destination
 * subnet, a gateway IPv4 address, and an outgoing interface. The administrator may create, update,
 * or delete records to/from the routing table. When a new client connects, it receives the entire
 * contents of the routing table. Whenever the administrator performs a create/update/delete
 * operation, all connected clients are notified of the change and update their local copies of the
 * routing table accordingly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/socket.h>
#include <signal.h>
#include "utils.h"
#include "rtm.h"
#include "input.h"
#include "fd_set_mgmt.h"

#define SOCKET_PATH "/tmp/RoutingTableSocket"  // master (connection) socket path
#define BACKLOG 128                            // maximum number of pending connections
#define BUF_SIZE 32                            // size of input sent by the admin to the RTM

int connection_socket;                         // master socket
RoutingTable *rtm;                             // routing table manager

void initialize_server();
void show_routing_menu(RoutingTable *rtm);
void handle_connection_initiation_request();
void handle_admin_input(RoutingTable *rtm, char *buffer, INPUT_STATE *state, ENTRY_TYPE *entry,
        msg_body_t *record);
void shutdown_server(int sig);


int main()
{
    initialize_server();
    rtm = routing_table_create();
    msg_body_t record;
    INPUT_STATE state = IDLE;
    ENTRY_TYPE entry;
    fd_set readfds;
    char buffer[BUF_SIZE];

    while (1) {
        refresh_fd_set(&readfds);
        if (state == IDLE) show_routing_menu(rtm);
        status_message("Waiting on select() system call.");
        select(get_max_fd() + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(connection_socket, &readfds)) {
            handle_connection_initiation_request();
        } else if (FD_ISSET(0, &readfds)) {
            memset(buffer, 0, BUF_SIZE);
            if (read(0, buffer, BUF_SIZE) == -1)
                error_and_exit("Cannot read admin input.");
            handle_admin_input(rtm, buffer, &state, &entry, &record);
        }
    }
}


// Initializes the routing table (server) process
void initialize_server()
{
    unlink(SOCKET_PATH);         // remove socket in case the program exited abnormally
    initialize_monitor_fd_set(); // initialize monitored fd_set
    add_to_monitored_fd_set(0);  // add STDIN to monitored file descriptors

    // Create master (connection) socket
    connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (connection_socket == -1)
        error_and_exit("Cannot create master socket.");
    status_message("Master socket created.");

    // Construct server socket address
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // Bind socket to socket address
    if (bind(connection_socket, (struct sockaddr *) &addr, sizeof(addr)) == -1)
        error_and_exit("Cannot bind socket.");
    status_message("Master socket bound to socket address.");

    // Make the master socket a listening socket
    if (listen(connection_socket, BACKLOG) == -1)
        error_and_exit("Cannot set master socket as listening socket.");

    add_to_monitored_fd_set(connection_socket); // add master socket to monitored fd_set
    signal(SIGINT, shutdown_server); // register SIGINT handler to shutdown server cleanly
    signal(SIGPIPE, SIG_IGN); // ignore SIGPIPE: when client disconnects the server keeps running
}


// Displays the admin routing menu
void show_routing_menu(RoutingTable *rtm)
{
    if (rtm->size == 0) {
        printf("The routing table has no entries. Available options:\n");
        printf("\tc - Create a record\n");
        printf("\tq - Quit\n");
    } else {
        printf("The routing table has %d %s. Available options:\n",
               rtm->size, rtm->size == 1 ? "entry" : "entries");
        printf("\tc - Create a record\n");
        printf("\tu - Update a record\n");
        printf("\td - Delete a record\n");
        printf("\tp - Print the routing table contents\n");
        printf("\tq - Quit\n");
    }
}


// Accepts a new connection from a client
void handle_connection_initiation_request()
{
    status_message("New connection initiation request received.");
    int data_socket = accept(connection_socket, NULL, NULL);
    if (data_socket == -1)
        error_and_exit("Cannot accept client connection.");
    status_message("Connection accepted from client.");
    add_to_monitored_fd_set(data_socket);
}


// Handles console input in the routing table server process
void handle_admin_input(RoutingTable *rtm, char *buffer, INPUT_STATE *state, ENTRY_TYPE *entry,
                        msg_body_t *record)
{
    remove_newline(buffer);
    printf("Admin input: '%s'.\n", buffer);
    if (*state == IDLE) {
        if (strlen(buffer) != 1) {
            printf("Unknown option '%s'.\n", buffer);
            return;
        }

        if (rtm->size == 0) {
            if (buffer[0] != 'c' && buffer[0] != 'C' && buffer[0] != 'q' && buffer[0] != 'Q') {
                printf("Unknown option '%c'.\n", buffer[0]);
                return;
            }
        }

        switch (buffer[0]) {
            case 'c':
            case 'C': {
                *state = CREATING;
                printf("Enter a new record:\n");
            };break;
            case 'u':
            case 'U': {
                *state = UPDATING;
                printf("Update an existing record:\n");
            }; break;
            case 'd':
            case 'D': {
                *state = DELETING;
                printf("Delete an existing record:\n");
            }; break;
            case 'p':
            case 'P': routing_table_print(rtm); break;
            case 'q':
            case 'Q': shutdown_server(SIGINT); break;
            default: printf("Unknown option '%c'.\n", buffer[0]);
        }
        printf("\tEnter destination subnet (xxx.xxx.xxx.xxx/yy): ");
        *entry = SUBNET;
        return;
    }

    switch (*state) {
        case CREATING:
            create_record(rtm, buffer, state, entry, record); break;
        case UPDATING:
            update_record(rtm, buffer, state, entry, record); break;
        case DELETING:
            delete_record(rtm, buffer, state, record); break;
        default: printf("Unknown state\n");
    }
}


// Shuts down the server (client processes are left running)
void shutdown_server(int sig)
{
    routing_table_free(rtm);
    if (close(connection_socket) == -1)
        error_and_exit("Cannot close master socket.");
    remove_from_monitored_fd_set(connection_socket);
    status_message("Connection closed.");
    if (unlink(SOCKET_PATH) == -1)
        error_and_exit("Cannot unlink master socket.");
    exit(EXIT_SUCCESS);
}
