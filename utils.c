//
// Created by Alexandra Zaharia on 10/02/19.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include "utils.h"


void error_message(char *message)
{
    fprintf(stderr, "%s%s%s\n", RED, message, RESET);
}


void error_and_exit(char *message)
{
    error_message(message);
    exit(EXIT_FAILURE);
}


void status_message(char *message)
{
    printf("%s%s%s\n", GREEN, message, RESET);
}


/*
 * Reads at most n characters (newline included) into the buffer. If present, the newline is
 * removed. Returns 0 on success and -1 on failure.
 */
int read_line(char *buffer, const int n)
{
    if (fgets(buffer, n, stdin) == NULL) return -1;

    char *p = strchr(buffer, '\n');
    if (p) {
        *p = '\0'; // replace newline with null character
    } else {
        while (getchar() != '\n'); // flush STDIN
    }

    return 0;
}


// Returns 0 if the buffer contains a valid integer, and -1 otherwise.
int _check_int(const char *buffer)
{
    char *end_ptr;
    long value = strtol(buffer, &end_ptr, 10);

    if (end_ptr == buffer) return -1;
    if (*end_ptr != '\0') return -1; // buffer ends with non-digit characters
    if ((value == LONG_MIN || value == LONG_MAX) && errno != 0) return -1;
    if (value < INT_MIN || value > INT_MAX) return -1;

    return 0;
}


// Attempts to read an integer into n from the buffer. Returns 0 on success and -1 on failure.
int read_int_from_buffer(const char *buffer, int *n)
{
    if (_check_int(buffer) == -1) return -1;
    *n = (int) strtol(buffer, NULL, 10);
    return 0;
}


// Attempts to read an integer into n from STDIN. Returns 0 on success and -1 on failure.
int read_int_from_stdin(int *n)
{
    char buffer[12];
    if (read_line(buffer, 12) == -1 || _check_int(buffer) == -1) return -1;
    *n = (int) strtol(buffer, NULL, 10);
    return 0;
}
