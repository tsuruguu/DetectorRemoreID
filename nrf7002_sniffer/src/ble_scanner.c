/* nrf7002_sniffer/src/ble_scanner.c */
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include "drone_id_types.hpp"

// Funkcja callback wywoływana przy każdym odebranym pakiecie BLE
static void device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t type,
			 struct net_buf_simple *ad)
{
    // 1. Sprawdź czy to pakiet Remote ID (UUID 0xFFFA)
    // 2. Jeśli tak, parsuj go używając opendroneid-core-c [3]
    // 3. Wypełnij strukturę DroneDiscoveryData
    // 4. Wywołaj send_to_brain()
}

int start_ble_sniffer(void) {
    struct bt_le_scan_param scan_param = {
       .type       = BT_LE_SCAN_TYPE_PASSIVE,
       .options    = BT_LE_SCAN_OPT_CODED | BT_LE_SCAN_OPT_EXTENDED, // Remote ID Long Range 
       .interval   = BT_GAP_SCAN_FAST_INTERVAL,
       .window     = BT_GAP_SCAN_FAST_WINDOW,
    };

    bt_enable(NULL);
    return bt_le_scan_start(&scan_param, device_found);
}