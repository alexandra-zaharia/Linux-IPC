//
// Created by Alexandra Zaharia on 10/02/19.
//

#ifndef LINUX_IPC_UTILS_H
#define LINUX_IPC_UTILS_H

#define RESET   "\033[0m"
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */

void error_message(char*);
void error_and_exit(char*);
void status_message(char*);
int read_int(int*);

#endif //LINUX_IPC_UTILS_H
