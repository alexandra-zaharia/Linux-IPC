//
// Created by Alexandra Zaharia on 20/03/19.
//

/*
 * This is a simple implementation of a server illustrating inter-process communication (IPC) via
 * POSIX message queues. The client process sends integers to the server process, until the value 0
 * is read. The server reads the integers from the message queue and sends the sum of the integer
 * back to the client process, via another message queue.
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <mqueue.h>
#include "utils.h"

#define MSG_QUEUE_PATH "/mqueue"   // message queue path
#define MAX_MESSAGES 10            // maximum number of messages in the message queue
#define QUEUE_PERMISSIONS 0660     // read and write access for user and group
#define INT_LEN 12                 // maximum number of characters for an integer

int mq_fd = -1;                    // message queue file descriptor

void shutdown_client(int sig);


int main()
{
    // Set queue attributes
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MESSAGES;
    attr.mq_curmsgs = 0;

    // Open message queue
    if ((mq_fd = mq_open(MSG_QUEUE_PATH, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror("mq_open");
        error_and_exit("Cannot open message queue.");
    }

    // Register SIGINT handler to shutdown server cleanly
    signal(SIGINT, shutdown_client);

    fd_set read_fds;
    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(mq_fd, &read_fds);
        select(mq_fd + 1, &read_fds, NULL, NULL, NULL);
        if (FD_ISSET(mq_fd, &read_fds)) {
            char input[INT_LEN];
            memset(input, 0, INT_LEN);
            if (mq_receive(mq_fd, input, INT_LEN, NULL) == -1) {
                perror("mq_receive");
                error_and_exit("Cannot read message from the message queue.");
            }
            printf("Message received from client: %s\n", input);

            int value;
            if (read_int_from_buffer(input, &value) == -1) {
                char err_msg[64];
                snprintf(err_msg, 64, "Ignoring message '%s' (not an integer).", input);
                error_message(err_msg);
            }
        }
    }
}


// Handles SIGINT (Ctrl+C) signal by closing the client connection
void shutdown_client(int sig)
{
    if (mq_close(mq_fd) == -1) {
        perror("mq_close");
        error_and_exit("Cannot close message queue.");
    }

    // Only server (receiver) process unlinks the queue, such that multiple clients are accepted
    if (mq_unlink(MSG_QUEUE_PATH) == -1) {
        perror("mq_unlink");
        error_and_exit("Cannot unlink message queue.");
    }

    exit(EXIT_SUCCESS);
}