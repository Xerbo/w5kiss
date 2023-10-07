#include "w5kiss.h"
#include <string.h>
#include <hardware/spi.h>
#include <hardware/flash.h>
#include <pico/binary_info.h>
#include <pico/unique_id.h>
#include "w5kiss_registers.h"

// Default to W5500-EVB-Pico pinout
#ifndef SPI_INSTANCE
#define SPI_INSTANCE spi0
#define PIN_SCLK     18
#define PIN_MOSI     19
#define PIN_MISO     16
#define PIN_CS       17
#define PIN_RST      20
#define PIN_INT      21
#endif

#define w5kiss_select() gpio_put(PIN_CS, false)
#define w5kiss_deselect() gpio_put(PIN_CS, true)

void w5kiss_read(uint8_t block, uint16_t address, uint8_t *data, size_t len) {
    w5kiss_select();

    uint8_t header[3];
    header[0] = address >> 8;
    header[1] = address & 0xFF;
    header[2] = block << 3;
    spi_write_blocking(SPI_INSTANCE, header, 3);
    spi_read_blocking(SPI_INSTANCE, 0x00, data, len);

    w5kiss_deselect();
}
uint8_t w5kiss_read_byte(uint8_t block, uint16_t address) {
    uint8_t data;
    w5kiss_read(block, address, &data, 1);
    return data;
}
uint16_t w5kiss_read_word(uint8_t block, uint16_t address) {
    return w5kiss_read_byte(block, address) << 8 | w5kiss_read_byte(block, address+1);
}
uint16_t w5kiss_read_word_safe(uint8_t block, uint16_t address) {
    while (true) {
        uint16_t a = w5kiss_read_word(block, address);
        uint16_t b = w5kiss_read_word(block, address);
        if (a == b) {
            return a;
        }
    }
}

void w5kiss_write(uint8_t block, uint16_t address, const uint8_t *data, size_t len) {
    w5kiss_select();

    uint8_t header[3];
    header[0] = address >> 8;
    header[1] = address & 0xFF;
    header[2] = (block << 3) | (1 << 2);
    spi_write_blocking(SPI_INSTANCE, header, 3);
    spi_write_blocking(SPI_INSTANCE, data, len);

    w5kiss_deselect();
}
void w5kiss_write_byte(uint8_t block, uint16_t address, uint8_t data) {
    w5kiss_write(block, address, &data, 1);
}
void w5kiss_write_word(uint8_t block, uint16_t address, uint16_t data) {
    w5kiss_write_byte(block, address, data >> 8);
    w5kiss_write_byte(block, address+1, data & 0xFF);
}

void w5kiss_sw_reset() {
    w5kiss_write_byte(BSB_COMMON, COMMON_MR, MR_RST);
    // TODO: is 100ms needed?
    sleep_ms(100);
}

bool w5kiss_init(uint frequency, uint8_t *mac) {
    spi_init(SPI_INSTANCE, frequency);
    gpio_set_function(PIN_SCLK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    bi_decl(bi_3pins_with_func(PIN_SCLK, PIN_MOSI, PIN_MISO, GPIO_FUNC_SPI));

    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, true);
    bi_decl(bi_1pin_with_name(PIN_CS, "w5k CS"));

    gpio_init(PIN_RST);
    gpio_set_dir(PIN_RST, GPIO_OUT);
    gpio_put(PIN_RST, true);
    bi_decl(bi_1pin_with_name(PIN_CS, "w5k RST"));

    // Reset
    w5kiss_sw_reset();

    // Set MAC address
    w5kiss_write(BSB_COMMON, COMMON_SHAR, mac, 6);

    // Set buffer sizes
    w5kiss_write_byte(BSB_SOCKET0, SOCKET_RXBUF_SIZE, 16);
    w5kiss_write_byte(BSB_SOCKET0, SOCKET_TXBUF_SIZE, 16);

    // Set to MACRAW (with MAC filter)
    w5kiss_write_byte(BSB_SOCKET0, SOCKET_MR, SMR_MACRAW);

    // Open socket
    w5kiss_write_byte(BSB_SOCKET0, SOCKET_CR, SCR_OPEN);

    // Check communication was successful
    uint8_t _mac[6];
    w5kiss_read(BSB_COMMON, COMMON_SHAR, _mac, 6);
    return memcmp(mac, _mac, 6) == 0;
}

void w5kiss_get_default_mac(uint8_t *mac) {
    pico_unique_board_id_t unique_id;
    pico_get_unique_board_id(&unique_id);

    // Raspberry Pi Trading Ltd
    mac[0] = 0xDC;
    mac[1] = 0xA6;
    mac[2] = 0x32;
    mac[3] = unique_id.id[0];
    mac[4] = unique_id.id[1];
    mac[5] = unique_id.id[2];
}

void w5kiss_get_current_mac(uint8_t *mac) {
    w5kiss_read(BSB_COMMON, COMMON_SHAR, mac, 6);
}

void _w5kiss_ignore(size_t len) {
    uint16_t ptr = w5kiss_read_word_safe(BSB_SOCKET0, SOCKET_RX_RD);
    w5kiss_write_word(BSB_SOCKET0, SOCKET_RX_RD, ptr + len);
}

void _w5kiss_receive(uint8_t *data, uint16_t len) {
    uint16_t ptr = w5kiss_read_word_safe(BSB_SOCKET0, SOCKET_RX_RD);
    w5kiss_read(BSB_SOCKET0_RX, ptr, data, len);
    w5kiss_write_word(BSB_SOCKET0, SOCKET_RX_RD, ptr + len);
}

uint16_t w5kiss_receive(uint8_t *data, uint16_t max_len) {
    // Ensure there is actually data
    if (w5kiss_read_word_safe(BSB_SOCKET0, SOCKET_RX_RSR) == 0) return 0;

    uint8_t header[2];
    _w5kiss_receive(header, 2);
    w5kiss_write_byte(BSB_SOCKET0, SOCKET_CR, SCR_RECV);
    uint16_t len = (header[0] << 8 | header[1]) - 2;

    if (len > max_len) {
        _w5kiss_ignore(len);
        w5kiss_write_byte(BSB_SOCKET0, SOCKET_CR, SCR_RECV);
        return 0;
    } else {
        _w5kiss_receive(data, len);
        w5kiss_write_byte(BSB_SOCKET0, SOCKET_CR, SCR_RECV);
        return len;
    }
}

uint16_t w5kiss_peek_length() {
    // Ensure there is actually data
    if (w5kiss_read_word_safe(BSB_SOCKET0, SOCKET_RX_RSR) == 0) return 0;

    uint8_t header[2];
    _w5kiss_receive(header, 2);
    return (header[0] << 8 | header[1]) - 2;
}

bool w5kiss_send(uint8_t *data, uint16_t len) {
    if (len > w5kiss_read_word_safe(BSB_SOCKET0, SOCKET_TX_FSR)) return false;

    uint16_t ptr = w5kiss_read_word_safe(BSB_SOCKET0, SOCKET_TX_WR);
    w5kiss_write(BSB_SOCKET0_TX, ptr, data, len);
    w5kiss_write_word(BSB_SOCKET0, SOCKET_TX_WR, ptr + len);
    w5kiss_write_byte(BSB_SOCKET0, SOCKET_CR, SCR_SEND);

    // Just assume the packet was sent correctly (it probably was)
    return true;
}

bool w5kiss_is_connected() {
    return w5kiss_read_byte(BSB_COMMON, COMMON_PHYCFGR) & PHYCFGR_LNK;
}
