//
// Created by Alexandra Zaharia on 08/04/19.
//

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include "shared_memory.h"
#include "utils.h"


void shutdown_reader(int sig);
void handle_missing_writer(int sig);
void *shm_region;
int shm_fd;

int main()
{
    shm_fd = shm_open(SHM_PATH, O_CREAT | O_RDONLY, SHM_PERMISSIONS);
    if (shm_fd == -1)
        error_and_exit("Cannot open shared memory.");

    shm_region = mmap(NULL, SHM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (shm_region == MAP_FAILED)
        error_and_exit("Cannot map memory.");

    signal(SIGINT, shutdown_reader);
    signal(SIGBUS, handle_missing_writer);

    int value = -1;       // value currently read from writer process
    int last_value = -1;  // last value read from writer process
    bool printed = false; // has the current value been displayed on screen?
    while (1) {
        char buffer[INT_LEN];
        memset(buffer, 0, INT_LEN);
        memcpy(buffer, shm_region, SHM_SIZE);
        if (read_int_from_buffer(buffer, &value) == -1)
            continue;
        if (last_value != value) {
            last_value = value;
            printed = false;
        }
        if (!printed) {
            printf("Received integer from writer process: %d\n", value);
            printed = true;
        }
    };
}


// Handles SIGINT (Ctrl+C) signal by closing the shared memory region
void shutdown_reader(int sig)
{
    if (munmap(shm_region, SHM_SIZE) == -1)
        error_and_exit("Cannot unmap memory.");

    if (close(shm_fd) == -1)
        error_and_exit("Cannot close shared memory.");

    if (shm_unlink(SHM_PATH) == -1)
        error_and_exit("Cannot unlink shared memory.");

    printf("Bye!\n");
    exit(EXIT_SUCCESS);
}


// Handles the case where the writer has not created the shared memory region
void handle_missing_writer(int sig)
{
    error_message("Specified memory region cannot be physically accessed.");
    error_message("(Has the memory region been truncated by another process?)");
    raise(SIGINT);
}