#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/sys/printk.h>
#include <opendroneid.h>
#include "drone_id_types.hpp"

extern void send_to_brain(struct DroneDiscoveryData *data);

static void device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t type,
                         struct net_buf_simple *ad)
{
    struct DroneDiscoveryData drone_data = {0};
    
    // Używamy samej części adresowej (17 znaków), aby zmieścić się w char[21]
    bt_addr_to_str(&addr->a, drone_data.serial_number, sizeof(drone_data.serial_number));
    
    drone_data.rssi = rssi;
    drone_data.protocol_type = 3; // BLE_Extended
    drone_data.timestamp = k_uptime_get_32();
    
    // Tu w przyszłości dodasz:
    // odid_decode_BasicID_Message(ad->data, &drone_data);

    send_to_brain(&drone_data);
}

int start_ble_sniffer(void) {
struct bt_le_scan_param scan_param = {
        .type       = BT_LE_SCAN_TYPE_PASSIVE,
        .options    = BT_LE_SCAN_OPT_CODED, // Zostawiamy samo CODED
        .interval   = BT_GAP_SCAN_FAST_INTERVAL,
        .window     = BT_GAP_SCAN_FAST_WINDOW,
    };

    int err = bt_enable(NULL);
    if (err) return err;

    return bt_le_scan_start(&scan_param, device_found);
}