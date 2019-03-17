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
#include "fd_set_mgmt.h"

#define SOCKET_PATH "/tmp/RoutingTableSocket"  // master (connection) socket path

int data_socket;                               // client socket

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
    while (1) {
        memset(&sync_msg, 0, sizeof(sync_msg_t));
        ssize_t bytes_read = read(data_socket, &sync_msg, sizeof(sync_msg_t));
        if (bytes_read == -1)
            error_and_exit("Error in reading synchronization message from the RTM server.");
        if (bytes_read == 0) break;
        printf("Received synchronization message from server: ");
        switch (sync_msg.op_code) {
            case CREATE: {
                printf("CREATE\n");
                printf("\tDestination subnet: '%s/%hu'\n",
                       sync_msg.msg_body.destination, sync_msg.msg_body.mask);
                printf("\tGateway: '%s'\n", sync_msg.msg_body.gateway_ip);
                printf("\tOutgoing interface: '%s'\n", sync_msg.msg_body.oif);
            };
                break;
            case UPDATE: {
                printf("UPDATE\n");
                printf("\tDestination subnet: '%s/%hu'\n",
                       sync_msg.msg_body.destination, sync_msg.msg_body.mask);
                printf("\tGateway: '%s'\n", sync_msg.msg_body.gateway_ip);
                printf("\tOutgoing interface: '%s'\n", sync_msg.msg_body.oif);
            };
                break;
            case DELETE: {
                printf("DELETE\n");
                printf("\tDestination subnet: '%s/%hu'\n",
                       sync_msg.msg_body.destination, sync_msg.msg_body.mask);
            };
                break;
        }
    }

    shutdown_client(SIGINT);
}


// Handles SIGINT (Ctrl+C) signal by closing the client connection
void shutdown_client(int sig)
{
    close(data_socket);
    remove_from_monitored_fd_set(data_socket);
    status_message("Connection closed.");

    exit(EXIT_SUCCESS);
}