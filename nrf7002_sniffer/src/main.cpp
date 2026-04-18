#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>
#include <zephyr/bluetooth/bluetooth.h> // DODAJ TO KONIECZNIE
#include <math.h> 
#include "inter_board_protocol.h"
#include "drone_id_types.hpp"

extern "C" {
    int start_wifi_sniffer(void);
    int start_ble_sniffer(void);
}

static const struct device *uart_brain = DEVICE_DT_GET(DT_NODELABEL(uart0));
K_MUTEX_DEFINE(uart_mutex);

/**
 * @brief Szacowanie odległości na podstawie RSSI.
 * @note To jest model log-distance path loss.
 */
float estimate_distance(int8_t rssi) {
    // Parametry można dostroić doświadczalnie
    float rssi_at_1m = -38.0f; // Typowa wartość dla drona 1m od odbiornika
    float n = 2.2f;            // Współczynnik propagacji (2.0 = próżnia)
    
    if (rssi >= 0) return 0.0f;
    return powf(10.0f, (rssi_at_1m - (float)rssi) / (10.0f * n));
}

/**
 * @brief Funkcja wysyłająca dane do głównego procesora (Brain) przez UART.
 */
extern "C" void send_to_brain(DroneDiscoveryData *data) {
    // 1. Obliczamy zmienne pomocnicze RAZ na początku funkcji
    float dist = estimate_distance(data->rssi);
    const char *proto = (data->protocol_type < 2) ? "Wi-Fi" : "BLE";

    // 2. LOGOWANIE DO KONSOLI (DEBUG)
    // To tutaj decydujesz, jak log wygląda w terminalu
    if (data->latitude != 0) {
        printk("[%s] DRONE DETECTED! | SN: %s | GPS: %.6f, %.6f | Alt: %.1fm | Dist: %.1fm\n",
               proto, data->serial_number, 
               (double)data->latitude / 10000000.0, 
               (double)data->longitude / 10000000.0,
               (double)data->altitude_msl / 100.0, 
               (double)dist);
    } else {
        // Log dla zwykłych sygnałów (tutaj będzie widać Twojego producenta i MAC)
        printk("[%s] Signal | ID: %s | RSSI: %d dBm | Dist: %.1fm\n", 
               proto, data->serial_number, data->rssi, (double)dist);
    }

    // 3. WYSYŁKA BINARNA DO PROCESORA "BRAIN" (UART)
    IpcHeader header = {
        .magic = PROTOCOL_MAGIC_BYTE,
        .type = MSG_TYPE_DRONE_DATA,
        .length = sizeof(DroneDiscoveryData)
    };
    
    uint8_t checksum = calculate_checksum((const uint8_t*)data, sizeof(DroneDiscoveryData));
    
    // if (k_mutex_lock(&uart_mutex, K_MSEC(100)) == 0) {
    //     uart_tx(uart_brain, (uint8_t*)&header, sizeof(header), SYS_FOREVER_US);
    //     k_busy_wait(1000); 
    //     uart_tx(uart_brain, (uint8_t*)data, sizeof(DroneDiscoveryData), SYS_FOREVER_US);
    //     k_busy_wait(2000);
    //     uart_tx(uart_brain, &checksum, 1, SYS_FOREVER_US);
    //     k_mutex_unlock(&uart_mutex);
    // }
}

int main(void) {
    k_msleep(1000);
    printk("\n--- nRF7002 RemoteID Sniffer v2.2 ---\n");

    if (!device_is_ready(uart_brain)) {
        printk("BŁĄD: UART brain nie jest gotowy!\n");
        return -1;
    }

    // Inicjalizacja Bluetooth przed snifferem!
    int bt_err = bt_enable(NULL);
    if (bt_err) {
        printk("BŁĄD: Inicjalizacja BT nieudana: %d\n", bt_err);
    } else {
        printk("Bluetooth OK.\n");
    }

    start_ble_sniffer();
    start_wifi_sniffer();

    printk("Skanowanie rozpoczęte...\n");

    while (1) { k_msleep(5000); }
}