//
// Created by Alexandra Zaharia on 12/02/19.
//

/*
 * This is a simple implementation of a server illustrating inter-process communication (IPC) via
 * stream-based UNIX domain sockets. The server receives integers from a client process, until the
 * client sends the value 0 to the server. At this point, the server sends to the client process
 * the sum of the integers.
 *
 * The server implementation is multiplexed, meaning that the server can handle more than one client
 * at a time.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "utils.h"
#include "fd_set_mgmt.h"

#define SOCKET_PATH "/tmp/DemoSocket"  // master (connection) socket path
#define BACKLOG 20                     // maximum size of pending connections
#define BUFFER_SIZE 128                // size of a message between client and server

int connection_socket;                 // master socket


void initialize_server();
void handle_connection_initiation_request();
void handle_service_request(fd_set read_fds, char* buffer);
void shutdown_server(int sig);


int main()
{
    initialize_server();

    fd_set read_fds;
    char buffer[BUFFER_SIZE];

    for (;;) { // handle client connections iteratively
        refresh_fd_set(&read_fds); // copy all file descriptors to read_fds

        // Server process blocks on select() system call
        status_message("Waiting on select() system call.");
        select(get_max_fd() + 1, &read_fds, NULL, NULL, NULL);

        if (FD_ISSET(connection_socket, &read_fds)) { // connection initiation request from client
            handle_connection_initiation_request();
        } else if (FD_ISSET(0, &read_fds)) { // console input on the server side
            memset(buffer, 0, BUFFER_SIZE);
            if (read(0, buffer, BUFFER_SIZE) == -1)
                error_and_exit("Cannot read data from console.");
            printf("Input read from console: %s\n", buffer);
        } else { // data arrives on client file descriptor
            handle_service_request(read_fds, buffer);
        }
    }
}


// Initializes the server
void initialize_server()
{
    // Remove the socket in case the program had previously exited abnormally
    unlink(SOCKET_PATH);

    // Initialize monitored fd_set
    initialize_monitor_fd_set();
    add_to_monitored_fd_set(0); // add STDIN to monitored file descriptors

    // Create the master (connection) socket
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
    if (bind(connection_socket, (struct sockaddr*) &addr, sizeof(struct sockaddr_un)) == -1)
        error_and_exit("Cannot bind socket.");
    status_message("Master socket bound to socket address.");

    // Make the master socket a listening socket
    if (listen(connection_socket, BACKLOG) == -1)
        error_and_exit("Cannot set master socket as listening socket.");

    // Add master socket to monitored fd_set
    add_to_monitored_fd_set(connection_socket);

    signal(SIGINT, shutdown_server); // register SIGINT handler to shutdown server cleanly
    signal(SIGPIPE, SIG_IGN); // ignore SIGPIPE: when client disconnects without waiting for message
}


// Accepts a new connection from a client
void handle_connection_initiation_request()
{
    status_message("New connection received, accepting the connection.");
    int data_socket = accept(connection_socket, NULL, NULL);
    if (data_socket == -1)
        error_and_exit("Cannot accept client connection.");
    status_message("Connection accepted from client.");
    add_to_monitored_fd_set(data_socket);
}


// Handles service requests from clients
void handle_service_request(fd_set read_fds, char* buffer)
{
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (FD_ISSET(monitored_fd_set[i], &read_fds)) {
            int comm_socket_fd = monitored_fd_set[i];

            // Prepare the buffer to receive the data
            memset(buffer, 0, BUFFER_SIZE);

            // Wait for next data packet ('read' is a blocking system call)
            status_message("Waiting for data from the client.");
            if (read(comm_socket_fd, buffer, BUFFER_SIZE) == -1)
                error_and_exit("Cannot read data from client.");

            // Add received data to sum
            int data;
            memcpy(&data, buffer, sizeof(int));
            if (data == 0) {
                // Send the result to the client
                memset(buffer, 0, BUFFER_SIZE);
                sprintf(buffer, "Result = %d", client_result[i]);
                status_message("Sending final result back to client.");
                if (write(comm_socket_fd, buffer, BUFFER_SIZE) == -1)
                    error_message("Cannot send sum to client.");

                // Close client socket
                close(comm_socket_fd);
                client_result[i] = 0;
                remove_from_monitored_fd_set(comm_socket_fd);
                continue; // go to select and block
            }
            client_result[i] += data;
        }
    }
}


// Handles SIGINT (Ctrl+C) signal by closing the server (client processes are left running)
void shutdown_server(int sig)
{
    // Close master socket and release socket resource
    close(connection_socket);
    remove_from_monitored_fd_set(connection_socket);
    status_message("Connection closed.");
    unlink(SOCKET_PATH);
    exit(EXIT_SUCCESS);
}