#line 1 "/Users/xewe/Documents/Programming/Arduino/XeWe-LedOS/lib_old/WiFi/extras/wifiHD/src/SOFTWARE_FRAMEWORK/SERVICES/LWIP/lwip-port-1.3.2/HD/if/include/netif/wlif.h"
#ifndef __NETIF_NRWLANIF_H__
#define __NETIF_NRWLANIF_H__

#include "lwip/netif.h"
#include "lwip/err.h"

err_t wlif_init(struct netif *netif);
void wlif_poll(struct netif *netif);

#endif
