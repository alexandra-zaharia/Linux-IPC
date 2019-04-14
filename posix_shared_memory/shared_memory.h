//
// Created by Alexandra Zaharia on 08/04/19.
//

#ifndef LINUX_IPC_SHARED_MEMORY_H
#define LINUX_IPC_SHARED_MEMORY_H

#include <memory.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#define SHM_PATH "/shm_test"
#define SHM_PERMISSIONS 0660  // read and write access for user and group
#define SHM_SIZE 12
#define INT_LEN 12

#endif //LINUX_IPC_SHARED_MEMORY_H
