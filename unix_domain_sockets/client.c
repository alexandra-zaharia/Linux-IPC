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
#include "utils.h"

#define SOCKET_PATH "/tmp/DemoSocket"  // master (connection) socket path
#define BUFFER_SIZE 128                // size of a message between client and server

int main()
{
    // Create data socket
    int data_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (data_socket == -1)
        error_message("Cannot create client socket");

     // Construct server socket address
     struct sockaddr_un addr;
     memset(&addr, 0, sizeof(struct sockaddr_un));
     addr.sun_family = AF_UNIX;
     strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

     // Connect client socket to socket address
     if (connect(data_socket, (struct sockaddr*) &addr, sizeof(struct sockaddr_un)) == -1)
         error_message("Cannot connect to server (is the server running?)");

     // Send integers for summation
     int value;
     do {
         printf("Enter integer to send to server (0 to stop): ");
         scanf("%d", &value);
         int num_written = write(data_socket, &value, sizeof(int));
         if (num_written == -1) {
             fprintf(stderr, RED "Error in sending data to server\n" RESET);
             break;
         }
         printf(GREEN "Number of bytes written: %d; data sent: %d\n" RESET, num_written, value);
     } while (value != 0);

     // Request result
     char buffer[BUFFER_SIZE];
     /*memset(buffer, 0, BUFFER_SIZE);
     strncpy(buffer, "Sum", strlen("Sum") + 1);
     buffer[strlen(buffer)] = '\0';
     printf(GREEN "Buffer: %s\n" RESET, buffer);

     if (write(data_socket, buffer, strlen(buffer)) == -1)
         error_message("Error in requesting result from server");*/

     // Receive result
     memset(buffer, 0, BUFFER_SIZE);
     if (read(data_socket, buffer, BUFFER_SIZE) == -1)
         error_message("Error in reading result from server");

     buffer[BUFFER_SIZE - 1] = '\0';
     printf(GREEN "%s\n" RESET, buffer);

     // Close socket
     close(data_socket);

     exit(EXIT_SUCCESS);
}