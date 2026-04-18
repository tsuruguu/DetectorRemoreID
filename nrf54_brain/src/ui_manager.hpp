/* nrf54_brain/src/ui_manager.cpp */
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>
#include "ui_manager.hpp"

// Tabela kodów dla wyświetlacza 7-segmentowego (0-9) przez 74HC595
static const uint8_t seg_codes = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

void UiManager::update_display(float distance, uint8_t protocol) {
    uint8_t d = (uint8_t)distance % 10;
    struct spi_buf buf = {.buf = (void *)&seg_codes[d],.len = 1 };
    struct spi_buf_set tx = {.buffers = &buf,.count = 1 };
    
    // Wysyłanie danych do 74HC595 przez SPI [3, 4]
    spi_write_dt(&spi_spec, &tx);
}

void UiManager::alert_proximity() {
    // Zapalenie czerwonej diody LED (P2.09 na nRF54L15-DK) 
    gpio_pin_set_dt(&led_red, 1);
}