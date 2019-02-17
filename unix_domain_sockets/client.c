//
// Created by Alexandra Zaharia on 10/02/19.
//

/* This is a simple implementation of a client illustrating inter-process communication (IPC) via
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
#include <signal.h>
#include <errno.h>
#include "utils.h"

#define SOCKET_PATH "/tmp/DemoSocket"  // master (connection) socket path
#define BUFFER_SIZE 128                // size of a message between client and server
#define INT_LEN 12                     // maximum number of characters for an integer

int data_socket;                       // client socket


// Handles SIGINT (Ctrl+C) signal by closing the client connection
void shutdown_client(int sig)
{
    close(data_socket);
    status_message("Connection closed.");

    exit(EXIT_SUCCESS);
}


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
    if (connect(data_socket, (struct sockaddr*) &addr, sizeof(struct sockaddr_un)) == -1)
     error_and_exit("Cannot connect to server (is the server running?).");

    signal(SIGINT, shutdown_client); // register SIGINT handler to shutdown client cleanly
    signal(SIGPIPE, SIG_IGN); // ignore SIGPIPE (server disconnects)

    // Send integers for summation
    int value = -1;
    do {
        char input[INT_LEN];
        printf("Enter integer to send to server (0 to stop): ");
        if (read_line(input, INT_LEN) == -1) {
            error_message("EOF, raising SIGINT.");
            raise(SIGINT);
        }
        if (read_int_from_buffer(input, &value) == -1) {
            error_message("Not an integer. Try again.");
            continue;
        };
        ssize_t num_written = write(data_socket, &value, sizeof(int));
        if (num_written == -1) {
            error_message("Error in sending data to server.");
            if (errno == EPIPE) {
                error_message("Server has disconnected. Quitting.");
                exit(EXIT_FAILURE);
            }
            break;
        }
        char write_successful[64];
        sprintf(write_successful,
                "Number of bytes written: %zu; data sent: %d", num_written, value);
        status_message(write_successful);
    } while (value != 0);

    // Receive result
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    if (read(data_socket, buffer, BUFFER_SIZE) == -1)
     error_and_exit("Error in reading result from server.");
    buffer[BUFFER_SIZE - 1] = '\0';
    status_message(buffer);

    // Close socket
    shutdown_client(SIGINT);
}