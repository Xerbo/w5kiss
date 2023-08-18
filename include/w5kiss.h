#ifndef _W5KISS_H
#define _W5KISS_H

#include <pico/stdlib.h>
#include <hardware/spi.h>

#define SPI_INSTANCE spi0
#define PIN_SCLK     2
#define PIN_MOSI     3
#define PIN_MISO     4
#define PIN_CS       5
#define PIN_RST      6
#define PIN_INT      7

#define w5kiss_init_default_mac(frequency) { \
    uint8_t mac[6]; \
    w5kiss_get_default_mac(mac); \
    w5kiss_init(frequency, mac); \
}

void w5kiss_init(uint frequency, uint8_t *mac);
void w5kiss_get_default_mac(uint8_t *mac);
void w5kiss_get_current_mac(uint8_t *mac);

bool w5kiss_send(uint8_t *data, uint16_t len);
uint16_t w5kiss_receive(uint8_t *data, uint16_t max_len);

#endif
