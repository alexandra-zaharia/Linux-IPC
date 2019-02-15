//
// Created by Alexandra Zaharia on 10/02/19.
//

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include "utils.h"

void error_message(char* message)
{
    fprintf(stderr, "%s%s%s\n", RED, message, RESET);
}

void error_and_exit(char* message)
{
    error_message(message);
    exit(EXIT_FAILURE);
}

void status_message(char* message)
{
    printf("%s%s%s\n", GREEN, message, RESET);
}

/*
 * Reads at most n characters (newline included) into str. If present, the newline is removed.
 * Returns the characters read (newline excluded).
 */
char* _s_gets(char* str, int n)
{
    char* ret_val = fgets(str, n, stdin);
    if (ret_val) {
        int i;
        for (i = 0; str[i] != '\n' && str[i] != '\0'; i++);
        if (str[i] == '\n') {
            str[i] = '\0';
        } else {
            while (getchar() != '\n')
                continue;
        }
    }
    return ret_val;
}

// Attempts to read an integer into n. Returns 0 on success and -1 on failure.
int read_int(int* n)
{
    char buffer[11];
    char* input = _s_gets(buffer, 11);
    char* end_ptr;
    long value = strtol(input, &end_ptr, 10);

    if (end_ptr == input) return -1;
    if ((value == LONG_MIN || value == LONG_MAX) && errno != 0) return -1;
    if (value < INT_MIN || value > INT_MAX) return -1;

    *n = (int) value;
    return 0;
}