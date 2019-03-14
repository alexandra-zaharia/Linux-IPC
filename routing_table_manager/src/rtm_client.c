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
#include "linked_list.h"
#include "rtm.h"

#define SOCKET_PATH "/tmp/RoutingTableSocket"  // master (connection) socket path

int data_socket;                               // client socket


void shutdown_client(int sig);


int main()
{
    // Create data socket
    data_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (data_socket == -1)
        error_and_exit("Cannot create client socket.");

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
    sync_msg_t *sync_msg = NULL;
    if (read(data_socket, sync_msg, sizeof(sync_msg_t *)) == -1)
        error_and_exit("Error in reading synchronization message from the RTM server.");

    // Close socket
    shutdown_client(SIGINT);
}


// Handles SIGINT (Ctrl+C) signal by closing the client connection
void shutdown_client(int sig)
{
    close(data_socket);
    status_message("Connection closed.");

    exit(EXIT_SUCCESS);
}