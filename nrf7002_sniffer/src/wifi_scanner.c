/* nrf7002_sniffer/src/wifi_scanner.c */
#include <zephyr/net/wifi_mgmt.h>
#include "drone_id_types.hpp"

static void wifi_mgmt_event_handler(struct net_mgmt_event_callback *cb,
				    uint32_t mgmt_event, struct net_if *iface)
{
    if (mgmt_event == NET_EVENT_WIFI_SCAN_RESULT) {
        const struct wifi_scan_result *res = (const struct wifi_scan_result *)cb->info;
        // Parsuj ramkę Beacon/NaN pod kątem danych Remote ID
        // nRF7002 przekaże RSSI oraz surowe dane ramki 
    }
}

int start_wifi_sniffer(void) {
    struct net_if *iface = net_if_get_default();
    // Konfiguracja skanowania pasywnego (nasłuch bez wysyłania Probe Requests)
    return wifi_mgmt_scan(iface, NULL);
}