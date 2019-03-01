//
// Created by Alexandra Zaharia on 13/02/19.
//

#include <stdio.h>
#include "utils.h"
#include "rtm.h"
#include "input.h"

void show_routing_menu(RoutingTable* rtm);
char read_routing_menu_choice(RoutingTable *rtm);


int main()
{
    RoutingTable* rtm = routing_table_create();
    show_routing_menu(rtm);
    routing_table_free(rtm);
}


void show_routing_menu(RoutingTable* rtm)
{
    char option;
    do {
        option = read_routing_menu_choice(rtm);
        switch (option) {
            case 'c':
            case 'C': create_record(rtm); break;
            case 'u':
            case 'U': {
                if (rtm->size == 0) {
                    printf("Unknown option '%c'.\n", option);
                    break;
                }
                printf("[updating a record...]\n");
            }; break;
            case 'd':
            case 'D': {
                if (rtm->size == 0) {
                    printf("Unknown option '%c'.\n", option);
                    break;
                }
                printf("[deleting a record...]\n");
            }; break;
            case 'p':
            case 'P': {
                if (rtm->size == 0) {
                    printf("Unknown option '%c'.\n", option);
                    break;
                }
                printf("[printing table...]\n");
            }; break;
            case 'q':
            case 'Q': printf("Bye!\n"); break;
            default: printf("Unknown option '%c'.\n", option);
        }
    } while (option != 'q' && option != 'Q');
}

char read_routing_menu_choice(RoutingTable *rtm)
{
    if (rtm->size == 0) {
        printf("The routing table has no entries. Available options:\n");
        printf("\tc - Create a record\n");
        printf("\tq - Quit\n");
    } else {
        printf("The routing table has %d %s. Available options:\n",
               rtm->size, rtm->size == 1 ? "entry" : "entries");
        printf("\tc - Create a record\n");
        printf("\tu - Update a record\n");
        printf("\td - Delete a record\n");
        printf("\tp - Print the routing table contents\n");
        printf("\tq - Quit\n");
    }

    char buffer[2];
    return read_line(buffer, 2) == -1 ? (char) 'q' : buffer[0];
}