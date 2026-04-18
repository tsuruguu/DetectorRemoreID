#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/net_if.h>
#include "drone_id_types.hpp"
#include <opendroneid.h>
#include <stdio.h>  

extern void send_to_brain(struct DroneDiscoveryData *data);

static void wifi_mgmt_event_handler(struct net_mgmt_event_callback *cb,
                                    uint32_t mgmt_event, struct net_if *iface)
{
    if (mgmt_event == NET_EVENT_WIFI_SCAN_RESULT) {
        const struct wifi_scan_result *res = (const struct wifi_scan_result *)cb->info;
        
        struct DroneDiscoveryData drone_data = {0};
        // Formatujemy MAC jako czytelny serial
        snprintf(drone_data.serial_number, 21, "%02X:%02X:%02X:%02X:%02X:%02X", 
                 res->mac[0], res->mac[1], res->mac[2], res->mac[3], res->mac[4], res->mac[5]);
        
        drone_data.rssi = res->rssi;
        drone_data.protocol_type = 0; // WiFi_Beacon
        drone_data.timestamp = k_uptime_get_32();
        
        send_to_brain(&drone_data);
    } 
    else if (mgmt_event == NET_EVENT_WIFI_SCAN_DONE) {
        // Gdy skanowanie się skończy, odpalamy je ponownie po krótkiej przerwie
        k_sleep(K_MSEC(500));
        struct wifi_scan_params params = { .scan_type = WIFI_SCAN_TYPE_PASSIVE };
        net_mgmt_call_wait(NET_REQUEST_WIFI_SCAN, iface, &params, sizeof(params), K_NO_WAIT);
    }
}

static struct net_mgmt_event_callback wifi_cb;

int start_wifi_sniffer(void) {
    struct net_if *iface = net_if_get_default();
    
    net_mgmt_init_event_callback(&wifi_cb, wifi_mgmt_event_handler, 
                                 NET_EVENT_WIFI_SCAN_RESULT | NET_EVENT_WIFI_SCAN_DONE);
    net_mgmt_add_event_callback(&wifi_cb);

    struct wifi_scan_params params = { .scan_type = WIFI_SCAN_TYPE_PASSIVE };
    printk("Wi-Fi Sniffer active (Passive scan)\n");
    return net_mgmt_call_wait(NET_REQUEST_WIFI_SCAN, iface, &params, sizeof(params), K_NO_WAIT);
}