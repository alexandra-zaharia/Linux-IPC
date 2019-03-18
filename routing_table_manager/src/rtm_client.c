//
// Created by Alexandra Zaharia on 13/02/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include "utils.h"
#include "rtm.h"

#define SOCKET_PATH "/tmp/RoutingTableSocket"  // master (connection) socket path

int data_socket;                               // client socket
RoutingTable *rtm;                             // routing table copy, local to this client

void synchronize_with_server(sync_msg_t sync_msg);
void shutdown_client(int sig);


int main()
{
    // Create data socket
    data_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (data_socket == -1)
        error_and_exit("Cannot create client socket.");
    printf("Data socket: %d\n", data_socket);

    // Construct server socket address
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // Connect client socket to socket address
    if (connect(data_socket, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1)
        error_and_exit("Cannot connect to server (is the server running?).");

    signal(SIGINT, shutdown_client); // register SIGINT handler to shutdown client cleanly
    signal(SIGPIPE, SIG_IGN);        // ignore SIGPIPE (i.e. when server disconnects)

    // Receive synchronization messages from the RTM server
    sync_msg_t sync_msg;
    rtm = routing_table_create();

    while (1) {
        memset(&sync_msg, 0, sizeof(sync_msg_t));
        ssize_t bytes_read = read(data_socket, &sync_msg, sizeof(sync_msg_t));
        if (bytes_read == -1)
            error_and_exit("Error in reading synchronization message from the RTM server.");
        if (bytes_read == 0) break; // end of file
        synchronize_with_server(sync_msg);
        routing_table_print(rtm);
    }

    shutdown_client(SIGINT);
}


/*
 * Handles a synchronization message received from the server by updating the local copy of the
 * routing table manager accordingly.
 */
void synchronize_with_server(sync_msg_t sync_msg)
{
    char msg_rcvd[64];
    switch (sync_msg.op_code) {
        case CREATE: {
            snprintf(msg_rcvd, 64, "Received synchronization message from server: CREATE");
            status_message(msg_rcvd);
            if (routing_table_insert(rtm, &sync_msg.msg_body) == -1)
                error_message("Could not insert record.");
        }; break;
        case UPDATE: {
            snprintf(msg_rcvd, 64, "Received synchronization message from server: UPDATE");
            status_message(msg_rcvd);
            if (routing_table_update(rtm, &sync_msg.msg_body) == -1)
                error_message("Could not update record.");
        }; break;
        case DELETE: {
            snprintf(msg_rcvd, 64, "Received synchronization message from server: DELETE");
            status_message(msg_rcvd);
            if (routing_table_delete(rtm, &sync_msg.msg_body) == -1)
                error_message("Could not delete record.");
        }; break;
    }
}


// Handles SIGINT (Ctrl+C) signal by closing the client connection
void shutdown_client(int sig)
{
    routing_table_free(rtm);
    close(data_socket);
    status_message("Connection closed.");
    exit(EXIT_SUCCESS);
}