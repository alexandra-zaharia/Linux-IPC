//
// Created by Alexandra Zaharia on 16/02/19.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "rtm.h"
#include "input.h"
#include "utils.h"


/*
 * Reads an IPv4 address from STDIN and stores it into the `ip_address` buffer, in the form
 * xxx.xxx.xxx.xxx. Returns 0 on success and -1 on failure.
 *
 * In order for an IPv4 address to be valid, it needs:
 *     - to contain precisely 3 '.' (dot) symbols
 *     - the 3 dots need to delimit precisely 4 groups of digits
 *     - each group of digits needs to be an integer between 0 and 255
 */
int read_ip_address_from_stdin(char *ip_address)
{
    if (read_line(ip_address, IP_ADDR_LEN) == -1) return -1;
    return read_ip_address_from_buffer(ip_address);
}


/*
 * Reads an IPv4 address from the `ip_address` buffer, in the form xxx.xxx.xxx.xxx. Returns 0 on
 * success and -1 on failure. Valid IPv4 addresses follow the same rules as for
 * `read_ip_address_from_stdin`.
 */
int read_ip_address_from_buffer(char *ip_address)
{
    if (!ip_address || strchr(ip_address, '.') == NULL) return -1;

    /* Create a copy of the input string to work on (`buffer`), since it gets modified by `strsep`.
     * A reference to the beginning of the buffer needs to be kept (`head`) in order to ensure
     * correct de-allocation. */
    char *buffer = strndup(ip_address, IP_ADDR_LEN); // work on a copy of the input string
    char *head = buffer;                             // keep a reference to the head of the buffer
    char *token;
    int gp_count = 0;

    while ((token = strsep(&buffer, ".")) != NULL) {
        int ip_gp;
        if (read_int_from_buffer(token, &ip_gp) == -1) goto clean_fail;
        if (ip_gp < IP_ADDR_MIN || ip_gp > IP_ADDR_MAX) goto clean_fail;
        ++gp_count;
    }
    free(head);                                      // when here, `buffer` is NULL
    head = NULL;
    if (gp_count == IP_ADDR_GP) return 0;

clean_fail:
    if (head) free(head);
    return -1;
}


/*
 * Reads a destination subnet from the `dst_subnet` buffer in the form xxx.xxx.xxx.xxx/yy, where
 * xxx.xxx.xxx.xxx is an IPv4 address that follows the same rules as for
 * `read_ip_address_from_stdin`, and yy is the subnet mask (an integer between 0 and 32). The IPv4
 * address is stored in the `ip_address` buffer, and the subnet mask is stored in the u16 variable
 * pointed to by `mask`. Returns 0 on success and -1 on failure.
 */
int read_destination_subnet(char *dst_subnet, char *ip_address, u16 *mask)
{
    if (read_line(dst_subnet, IP_ADDR_LEN + 3) == -1) return -1;
    if (strchr(dst_subnet, '/') == NULL) return -1;

    char *token;
    int token_count = 0;

    while ((token = strsep(&dst_subnet, "/")) != NULL) {
        if (++token_count > 2) return -1;
        switch (token_count) {
            case 1: {
                char buffer[IP_ADDR_LEN];
                strncpy(buffer, token, IP_ADDR_LEN);
                if (read_ip_address_from_buffer(buffer) == -1) return -1;
                strncpy(ip_address, token, IP_ADDR_LEN);
            }; break;
            case 2: {
                int _mask;
                if (read_int_from_buffer(token, &_mask) == -1) return -1;
                if (_mask < MASK_MIN || _mask > MASK_MAX) return -1;
                *mask = (u16) _mask;
            }; break;
            default: return -1;
        }
    }

    return 0;
}


// Prompts the administrator to enter a new routing table record.
void create_record()
{
    printf("Enter a new record:\n");

    char sub_addr[IP_ADDR_LEN];
    char dst_subnet[IP_ADDR_LEN + 3];
    u16 mask;
    printf("\tEnter destination subnet (xxx.xxx.xxx.xxx/yy): ");
    while (read_destination_subnet(dst_subnet, sub_addr, &mask) == -1) {
        error_message("\tIncorrect destination subnet format. Try again.");
        printf("\tEnter destination subnet (xxx.xxx.xxx.xxx/yy): ");
    }

    char gw_addr[IP_ADDR_LEN];
    printf("\tEnter gateway IP (xxx.xxx.xxx.xxx): ");
    while (read_ip_address_from_stdin(gw_addr) == -1) {
        error_message("\tIncorrect IP address format. Try again.");
        printf("\tEnter gateway IP (xxx.xxx.xxx.xxx): ");
    }

    char oif[OIF_LEN];
    printf("\tEnter outgoing interface: ");
    while (read_line(oif, OIF_LEN) == -1) {
        error_message("\tInvalid outgoing interface. Try again.");
        printf("\tEnter outgoing interface: ");
    }

    printf("Adding record %s/%hu %s %s\n", sub_addr, mask, gw_addr, oif);
}
