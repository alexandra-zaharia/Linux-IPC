//
// Created by Alexandra Zaharia on 10/02/19.
//

/*
 * This is a simple implementation of a server illustrating inter-process communication (IPC) via
 * stream-based UNIX domain sockets. The server receives integers from a client process, until the
 * client sends the value 0 to the server. At this point, the server sends to the client process
 * the sum of the integers.
 *
 * The drawback of this basic client/server implementation is that the server can only handle one
 * client at a time. The workaround is to use a multiplexed UNIX domain server implementation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include "utils.h"

#define SOCKET_PATH "/tmp/DemoSocket"  // master (connection) socket path
#define BACKLOG 20                     // maximum size of pending connections
#define BUFFER_SIZE 128                // size of a message between client and server


int main()
{
    // Remove the socket in case the program had previously exited abnormally
    unlink(SOCKET_PATH);

    // Create the master (connection) socket
    int connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (connection_socket == -1)
        error_message("Cannot create master socket");
    status_message("Master socket created");

    // Construct server socket address
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // Bind socket to socket address
    if (bind(connection_socket, (struct sockaddr*) &addr, sizeof(struct sockaddr_un)) == -1)
        error_message("Cannot bind socket");
    status_message("Master socket bound to socket address");

    // Make the master socket a listening socket
    if (listen(connection_socket, BACKLOG) == -1)
        error_message("Cannot set master socket as listening socket");

    for (;;) { // handle client connections iteratively
        /* Accept a connection on a new socket ('data_socket'). The listening socket
         * ('connection_socket') remains open and can be used to accept further connections. */
        status_message("Waiting on accept() system call");
        int data_socket = accept(connection_socket, NULL, NULL);
        if (data_socket == -1)
            error_message("Cannot accept client connection");
        status_message("Connection accepted from client");

        int sum = 0;
        char buffer[BUFFER_SIZE];
        for (;;) {
            // Prepare the buffer to receive the data
            memset(buffer, 0, BUFFER_SIZE);

            // Wait for next data packet ('read' is a blocking system call)
            status_message("Waiting for data from the client");
            if (read(data_socket, buffer, BUFFER_SIZE) == -1)
                error_message("Cannot read data from client");

            // Add received data to sum
            int data;
            memcpy(&data, buffer, sizeof(int));
            if (data == 0)
                break;
            sum += data;
        }

        // Send the result to the client
        memset(buffer, 0, BUFFER_SIZE);
        sprintf(buffer, "Result = %d", sum);
        status_message("Sending final result back to client");
        if (write(data_socket, buffer, BUFFER_SIZE) == -1)
            error_message("Cannot send sum to client");

        // Close client socket
        close(data_socket);
    }

    // Close master socket and release socket resource
    close(connection_socket);
    status_message("Connection closed");
    unlink(SOCKET_PATH);

    exit(EXIT_SUCCESS);
}