/* nrf54_brain/src/data_aggregator.cpp */
#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include "drone_id_types.hpp"
#include "inter_board_protocol.h"

K_MSGQ_DEFINE(drone_msgq, sizeof(DroneDiscoveryData), 10, 4);
static const struct device *uart_dev = DEVICE_DT_GET(DT_NODELABEL(uart1));

// Prosta maszyna stanów do parsowania protokołu z common/
void uart_callback(const struct device *dev, struct uart_event *evt, void *user_data) {
    static DroneDiscoveryData rx_buf;
    static IpcHeader header;
    // Tutaj implementacja odbierania bajt po bajcie zgodnie z inter_board_protocol.h
    // Po odebraniu pełnej ramki:
    k_msgq_put(&drone_msgq, &rx_buf, K_NO_WAIT);
}

int data_aggregator_get_next(DroneDiscoveryData *data) {
    return k_msgq_get(&drone_msgq, data, K_NO_WAIT);
}