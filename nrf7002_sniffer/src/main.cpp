/* nrf7002_sniffer/src/main.cpp */
#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include "inter_board_protocol.h"
#include "drone_id_types.hpp"

extern "C" {
    int start_wifi_sniffer(void);
    int start_ble_sniffer(void);
}

static const struct device *uart_brain = DEVICE_DT_GET(DT_NODELABEL(uart0));

/**
 * @brief Wysyła dane o dronie do nRF54 przez UART
 */
void send_to_brain(DroneDiscoveryData *data) {
    IpcHeader header = {
       .magic = PROTOCOL_MAGIC_BYTE,
       .type = MSG_TYPE_DRONE_DATA,
       .length = sizeof(DroneDiscoveryData)
    };
    
    uint8_t checksum = calculate_checksum((uint8_t*)data, sizeof(DroneDiscoveryData));
    
    uart_tx(uart_brain, (uint8_t*)&header, sizeof(header), K_FOREVER);
    uart_tx(uart_brain, (uint8_t*)data, sizeof(DroneDiscoveryData), K_FOREVER);
    uart_tx(uart_brain, &checksum, 1, K_FOREVER);
}

void main(void) {
    printk("nRF7002 Remote ID Sniffer Starting...\n");
    
    start_ble_sniffer();
    start_wifi_sniffer();

    while (1) {
        k_msleep(1000); // Skanery działają w tle na callbackach
    }
}