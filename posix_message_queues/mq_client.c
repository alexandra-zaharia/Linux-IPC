//
// Created by Alexandra Zaharia on 20/03/19.
//

/*
 * This is a simple implementation of a client illustrating inter-process communication (IPC) via
 * POSIX message queues. The client process sends integers to the server process, until the value 0
 * is read. The server reads the integers from the message queue and prints them to STDOUT.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <mqueue.h>
#include "utils.h"

#define MSG_QUEUE_PATH "/mqueue"   // message queue path
#define INT_LEN 12                 // maximum number of characters for an integer

int mq_fd = -1;                    // message queue file descriptor

void shutdown_client(int sig);


int main()
{
    // Open message queue
    if ((mq_fd = mq_open(MSG_QUEUE_PATH, O_WRONLY | O_CREAT, 0, 0)) == -1) {
        perror("mq_open");
        error_and_exit("Cannot open message queue.");
    }

    // Register SIGINT handler to shutdown client cleanly
    signal(SIGINT, shutdown_client);

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
        if (value == 0) break;

        if (mq_send(mq_fd, input, strlen(input) + 1, 0) == -1) {
            perror("mq_send");
            error_message("Cannot place message in the message queue.");
        }
    } while (value != 0);

    shutdown_client(0);
}


// Handles SIGINT (Ctrl+C) signal by closing the client connection
void shutdown_client(int sig)
{
    if (mq_close(mq_fd) == -1) {
        perror("mq_close");
        error_and_exit("Cannot close message queue.");
    }

    exit(EXIT_SUCCESS);
}
