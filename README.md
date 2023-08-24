# w5kiss

w5kiss is a driver for the [WizNet W5500](https://www.wiznet.io/product-item/w5500/) following the [KISS principle](https://en.wikipedia.org/wiki/KISS_principle).

w5kiss is written for the Raspberry Pi Pico however should be easily adaptable to other platforms.

## Adding to a project

```
git submodule add https://github.com/Xerbo/w5kiss
```

Copy `lwipopts.h.example` into your project as `lwipopts.h` somewhere LwIP can find it.

And then include the w5kiss as a library:

```
add_subdirectory(w5kiss)
target_link_libraries(${PROJECT_NAME} ... w5kiss)
```

## Using

w5kiss uses the internal LwIP stack that comes with pico-sdk so adding apps can be done by linking `pico_lwip_xxxx`.

```c
#include <pico/stdlib.h>
#include <lwip/init.h>
#include <lwip/dhcp.h>
#include <lwip/timeouts.h>
#include <netif/ethernet.h>

#include "w5kiss.h"
#include "w5kiss_netif.h"

int main() {
    stdio_init_all();

    w5kiss_init_default_mac(5000000);
    lwip_init();

    struct netif netif;
    netif_add(&netif, NULL, NULL, NULL, NULL, &w5kiss_netif_init, &ethernet_input);
    netif_set_default(&netif);
    netif_set_up(&netif);
    dhcp_start(&netif);

    while (true) {
        w5kiss_netif_input(&netif);
        sys_check_timeouts();
    }
}
```

## License

w5kiss is a clean room implementation and contains no WizNet code, and is licensed under MIT.
