//
// Created by Alexandra Zaharia on 12/02/19.
//

#ifndef LINUX_IPC_FD_SET_MGMT_H
#define LINUX_IPC_FD_SET_MGMT_H

#include <sys/types.h>

#define MAX_CLIENTS 32 // maximum number of supported clients + 1 for the master socket

extern int monitored_fd_set[MAX_CLIENTS]; // file descriptors maintained by the server process
extern int client_result[MAX_CLIENTS]; // intermediary results for client processes

void initialize_monitor_fd_set(); // remove all file descriptors from the monitored fd_set
void add_to_monitored_fd_set(int); // add a new file descriptor to the monitored fd_set
void remove_from_monitored_fd_set(int); // remove a file descriptor from the monitored fd_set
void refresh_fd_set(fd_set *fd_set_ptr); // clone all file descriptors into the fd_est
int get_max_fd(); // return the file descriptor in the monitored fd_set with the max value

#endif //LINUX_IPC_FD_SET_MGMT_H
