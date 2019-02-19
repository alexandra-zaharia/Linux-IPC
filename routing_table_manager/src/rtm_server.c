//
// Created by Alexandra Zaharia on 13/02/19.
//

#include <stdio.h>
#include "utils.h"
#include "linked_list.h"
#include "rtm.h"
#include "input.h"

int main()
{
    printf("server\n");

    char ip_addr[IP_ADDR_LEN];
    printf("Enter IP address (xxx.xxx.xxx.xxx): ");
    printf("%s\n", read_ip_address_from_stdin(ip_addr) == 0 ? ip_addr : "invalid");

    char dst_subnet[IP_ADDR_LEN + 3];
    u16 mask;
    printf("Enter dst subnet (xxx.xxx.xxx.xxx/yy): ");
    if (read_destination_subnet(dst_subnet, ip_addr, &mask) == 0) {
        printf("%s\n%hu\n", ip_addr, mask);
    } else {
        printf("invalid\n");
    }

}