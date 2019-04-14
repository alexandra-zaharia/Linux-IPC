//
// Created by Alexandra Zaharia on 08/04/19.
//

#include <stdio.h>
#include <signal.h>
#include "shared_memory.h"
#include "utils.h"

int main()
{
    int shm_fd = shm_open(SHM_PATH, O_CREAT | O_RDWR | O_TRUNC, SHM_PERMISSIONS);
    if (shm_fd == -1)
        error_and_exit("Cannot create shared memory.");

    if (ftruncate(shm_fd, SHM_SIZE) == -1)
        error_and_exit("Failed to allocate the requested shared memory size.");

    void *shm_region = mmap(
        NULL,                   // let kernel choose base address
        SHM_SIZE,               // size of the shm to map to virtual address space of the process
        PROT_READ | PROT_WRITE, // shared memory is readable and writable
        MAP_SHARED,             // shared memory is accessible by different processes
        shm_fd,                 // file descriptor for the shared memory
        0);                     // offset from base address of the physical memory to be mapped
    if (shm_region == MAP_FAILED)
        error_and_exit("Cannot map memory.");

    int value = -1;
    do {
        char input[INT_LEN];
        printf("Enter integer to write in shared memory (0 to stop): ");
        if (read_line(input, INT_LEN) == -1) {
            error_message("EOF, raising SIGINT.");
            raise(SIGINT);
        }
        if (read_int_from_buffer(input, &value) == -1) {
            error_message("Not an integer. Try again.");
            continue;
        }
        if (value == 0) continue;

        memset(shm_region, 0, SHM_SIZE);
        memcpy(shm_region, input, SHM_SIZE);
    } while (value != 0);

    munmap(shm_region, SHM_SIZE);
    close(shm_fd);
}