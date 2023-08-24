#ifndef _W5KISS_NETIF_H
#define _W5KISS_NETIF_H

#include <pico/stdlib.h>
#include <lwip/netif.h>

err_t w5kiss_netif_init(struct netif *netif);
void w5kiss_netif_input(struct netif *netif);

#endif
