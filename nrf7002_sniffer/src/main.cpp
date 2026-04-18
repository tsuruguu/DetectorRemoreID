#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>
#include "inter_board_protocol.h"
#include "drone_id_types.hpp"

extern "C" {
    int start_wifi_sniffer(void);
    int start_ble_sniffer(void);
}

static const struct device *uart_brain = DEVICE_DT_GET(DT_NODELABEL(uart0));
K_MUTEX_DEFINE(uart_mutex); // Mutex chroniący dostęp do UART

void send_to_brain(DroneDiscoveryData *data) {
    IpcHeader header = {
        .magic = PROTOCOL_MAGIC_BYTE,
        .type = MSG_TYPE_DRONE_DATA,
        .length = sizeof(DroneDiscoveryData)
    };
    
    uint8_t checksum = calculate_checksum((uint8_t*)data, sizeof(DroneDiscoveryData));
    
    // Blokujemy UART dla jednego wątku
    k_mutex_lock(&uart_mutex, K_FOREVER);
    
    // Wysyłamy jako jeden ciągły blok danych, aby uniknąć przerw
    uart_tx(uart_brain, (uint8_t*)&header, sizeof(header), K_MSEC(50));
    k_msleep(5); // Krótka pauza dla stabilności DMA
    uart_tx(uart_brain, (uint8_t*)data, sizeof(DroneDiscoveryData), K_MSEC(100));
    k_msleep(10);
    uart_tx(uart_brain, &checksum, 1, K_MSEC(50));

    k_mutex_unlock(&uart_mutex);

    printk("[%s] Drone Detected: %s | RSSI: %d dBm\n", 
           (data->protocol_type < 2) ? "Wi-Fi" : "BLE", 
           data->serial_number, data->rssi);
}

int main(void) {
    k_msleep(1000); // Czas na ustabilizowanie się zasilania
    printk("--- nRF7002 Dual-Sniffer Started ---\n");

    if (!device_is_ready(uart_brain)) {
        printk("Error: UART not ready\n");
        return -1;
    }

    start_ble_sniffer();
    start_wifi_sniffer();

    while (1) {
        k_msleep(1000);
    }
}