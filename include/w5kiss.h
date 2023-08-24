#ifndef _W5KISS_H
#define _W5KISS_H

#include <pico/stdlib.h>
#include <hardware/spi.h>

bool w5kiss_init(uint frequency, uint8_t *mac);
void w5kiss_get_default_mac(uint8_t *mac);
void w5kiss_get_current_mac(uint8_t *mac);
inline bool w5kiss_init_default_mac(uint frequency) {
    uint8_t mac[6];
    w5kiss_get_default_mac(mac);
    return w5kiss_init(frequency, mac);
}

bool w5kiss_send(uint8_t *data, uint16_t len);
uint16_t w5kiss_receive(uint8_t *data, uint16_t max_len);
uint16_t w5kiss_peek_length();
bool w5kiss_is_connected();

#endif
