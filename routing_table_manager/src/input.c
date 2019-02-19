//
// Created by Alexandra Zaharia on 16/02/19.
//

#include <string.h>
#include <stdio.h>
#include "input.h"
#include "utils.h"


/*
 * Reads an IPv4 address from STDIN, in the format xxx.xxx.xxx.xxx. Returns 0 on success and -1 on
 * failure.
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
 * Reads an IPv4 address from the buffer, in the format xxx.xxx.xxx.xxx. Returns 0 on success and -1
 * on failure. Valid IPv4 addresses follow the same rules as for read_ip_address_from_stdin.
 */
int read_ip_address_from_buffer(char *ip_address)
{
    if (!ip_address) return -1;
    if (strchr(ip_address, '.') == NULL) return -1;

    char buffer[IP_ADDR_LEN];
    strncpy(buffer, ip_address, IP_ADDR_LEN);
    char *token = strtok(buffer, ".");
    int gp_count = 0;

    while (token) {
        int ip_gp;
        if (read_int_from_buffer(token, &ip_gp) == -1) return -1;
        if (ip_gp < IP_ADDR_MIN || ip_gp > IP_ADDR_MAX) return -1;
        ++gp_count;
        token = strtok(NULL, ".");
    }

    if (gp_count != IP_ADDR_GP) return -1;

    return 0;
}


/*
 * Reads a destination subnet in the form xxx.xxx.xxx.xxx/yy, where xxx.xxx.xxx.xxx is an IPv4
 * address that follows the same rules as for read_ip_address_from_stdin, and yy is the subnet mask
 * (an integer between 0 and 32). Returns 0 on success and -1 on failure.
 */
int read_destination_subnet(char *dst_subnet, char *ip_address, u16 *mask)
{
    if (read_line(dst_subnet, IP_ADDR_LEN + 3) == -1) return -1;
    if (strchr(dst_subnet, '/') == NULL) return -1;

    // Get IP address
    char *token = strtok(dst_subnet, "/");
    strncpy(ip_address, token, IP_ADDR_LEN);

    // Get and test mask
    token = strtok(NULL, "/");
    int _mask;
    if (read_int_from_buffer(token, &_mask) == -1) return -1;
    if (_mask < MASK_MIN || _mask > MASK_MAX) return -1;
    *mask = (u16) _mask;

    // Make sure there is nothing after the mask
    if (strtok(NULL, "/")) return -1;

    // Test IP address
    char buffer[IP_ADDR_LEN];
    strncpy(buffer, ip_address, IP_ADDR_LEN);
    if (read_ip_address_from_buffer(buffer) == -1) return -1;

    return 0;
}