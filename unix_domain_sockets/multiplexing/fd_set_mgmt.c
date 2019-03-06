//
// Created by Alexandra Zaharia on 12/02/19.
//

#include "fd_set_mgmt.h"

int monitored_fd_set[MAX_CLIENTS];
int client_result[MAX_CLIENTS] = {0};

// Remove all file descriptors (if any) from the monitored fd_set
void initialize_monitor_fd_set()
{
    for (int i = 0; i < MAX_CLIENTS; i++)
        monitored_fd_set[i] = -1;
}


// Add a new file descriptor to the monitored fd_set
void add_to_monitored_fd_set(int socket_fd)
{
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (monitored_fd_set[i] != -1)
            continue;
        monitored_fd_set[i] = socket_fd;
        break;
    }
}


// Remove a file descriptor from the monitored fd_set
void remove_from_monitored_fd_set(int socket_fd)
{
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (monitored_fd_set[i] == socket_fd) {
            monitored_fd_set[i] = -1;
            break;
        }
    }
}


// Clone all file descriptors from the monitored file descriptor set into the fd_set
void refresh_fd_set(fd_set *fd_set_ptr)
{
    FD_ZERO(fd_set_ptr);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (monitored_fd_set[i] != -1)
            FD_SET(monitored_fd_set[i], fd_set_ptr);
    }
}


// Return the file descriptor in the monitored fd_set with the max value
int get_max_fd()
{
    int max = -1;

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (monitored_fd_set[i] > max)
            max = monitored_fd_set[i];
    }

    return max;
}
