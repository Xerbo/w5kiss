#include "w5kiss_netif.h"
#include <string.h>
#include <lwip/etharp.h>
#include "w5kiss.h"

static err_t w5kiss_netif_output(struct netif *netif, struct pbuf *p) {
    return w5kiss_send(p->payload, p->tot_len) ? ERR_OK : ERR_IF;
}

err_t w5kiss_netif_init(struct netif *netif) {
    netif->linkoutput = w5kiss_netif_output;
    netif->output     = etharp_output;
    netif->mtu        = 1500;
    netif->flags      = NETIF_FLAG_BROADCAST | NETIF_FLAG_LINK_UP | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET;
    netif->name[0]    = 'w';
    netif->name[1]    = '5';
    netif->hostname   = "Pico";

    w5kiss_get_current_mac(netif->hwaddr);
    netif->hwaddr_len = 6;

    return ERR_OK;
}

void w5kiss_netif_input(struct netif *netif) {
    uint16_t len = w5kiss_peek_length();
    if (len != 0) {
        struct pbuf *p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
        w5kiss_receive(p->payload, p->tot_len);
        if (netif->input(p, netif) != ERR_OK) {
            pbuf_free(p);
        }
    }
}
