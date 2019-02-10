//
// Created by Alexandra Zaharia on 10/02/19.
//

#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

void error_message(char* message)
{
    fprintf(stderr, "%s%s%s\n", RED, message, RESET);
    exit(EXIT_FAILURE);
}

void status_message(char* message)
{
    printf("%s%s%s\n", GREEN, message, RESET);
}