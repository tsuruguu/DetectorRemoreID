#ifndef CONFIG_WIFI_MGMT_SCAN_SSID_ADVANCED
#define CONFIG_WIFI_MGMT_SCAN_SSID_ADVANCED 1
#endif

#include <zephyr/kernel.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/net_if.h>
#include <stdio.h>
#include <string.h>
#include "drone_id_types.hpp"
#include <opendroneid.h> 

extern void send_to_brain(struct DroneDiscoveryData *data);

// DEFINICJA MUSI BYĆ NA GÓRZE
static const char* identify_manufacturer(const uint8_t *mac) {
    // 1. Bit losowości - sprawdzamy na końcu, żeby najpierw spróbować dopasować realne OUI
    // (Niektóre firmy używają wielu zakresów, w tym nietypowych)

    // --- DRONY I KONTROLERY ---
    if (mac[0] == 0x60 && mac[1] == 0x60 && mac[2] == 0x1F) return "DJI";
    if (mac[0] == 0x00 && mac[1] == 0x26 && mac[2] == 0x7E) return "DJI";
    if (mac[0] == 0x34 && mac[1] == 0xD2 && mac[2] == 0x62) return "DJI (Mavic)";
    if (mac[0] == 0x00 && mac[1] == 0xE0 && mac[2] == 0x4C) return "DJI (Realtek-Base)";
    if (mac[0] == 0x00 && mac[1] == 0x12 && mac[2] == 0x1C) return "Parrot";
    if (mac[0] == 0x90 && mac[1] == 0x03 && mac[2] == 0xB7) return "Parrot (Skycontroller)";
    if (mac[0] == 0x00 && mac[1] == 0x0B && mac[2] == 0xE5) return "Autel Robotics";
    if (mac[0] == 0x08 && mac[1] == 0xD5 && mac[2] == 0x39) return "FIMI (Xiaomi)";
    if (mac[0] == 0x40 && mac[1] == 0x15 && mac[2] == 0xFF) return "Hubsan";
    if (mac[0] == 0x10 && mac[1] == 0x04 && mac[2] == 0xB8) return "Skydio";
    if (mac[0] == 0x04 && mac[1] == 0xEE && mac[2] == 0x03) return "Yuneec";

    // --- SPRZĘT MOBILNY / TELEFONY ---
    // Apple (częste zakresy)
    if (mac[0] == 0xAC && mac[1] == 0x3C && mac[2] == 0x0B) return "Apple (iPhone)";
    if (mac[0] == 0xF0 && mac[1] == 0x18 && mac[2] == 0x98) return "Apple";
    if (mac[0] == 0x8C && mac[1] == 0x85 && mac[2] == 0x90) return "Apple";
    
    // Samsung
    if (mac[0] == 0x3E && mac[1] == 0x36 && mac[2] == 0xF0) return "Samsung";
    if (mac[0] == 0x50 && mac[1] == 0x85 && mac[2] == 0x69) return "Samsung";
    if (mac[0] == 0x24 && mac[1] == 0xFC && mac[2] == 0xE5) return "Samsung";

    // Xiaomi / Huawei / Oppo
    if (mac[0] == 0x64 && mac[1] == 0x09 && mac[2] == 0x80) return "Xiaomi";
    if (mac[0] == 0x28 && mac[1] == 0x6D && mac[2] == 0xCD) return "Xiaomi";
    if (mac[0] == 0x40 && mac[1] == 0x4D && mac[2] == 0x7F) return "Huawei";
    if (mac[0] == 0x34 && mac[1] == 0xCE && mac[2] == 0x00) return "Huawei";

    // --- MODUŁY I IOT (Dla dłubaczy dornów DIY) ---
    if (mac[0] == 0x24 && mac[1] == 0xA5 && mac[2] == 0x2C) return "Espressif (ESP32)";
    if (mac[0] == 0x30 && mac[1] == 0xAE && mac[2] == 0xA4) return "Espressif (ESP32)";
    if (mac[0] == 0xDC && mac[1] == 0xA6 && mac[2] == 0x32) return "Raspberry Pi";
    if (mac[0] == 0xB8 && mac[1] == 0x27 && mac[2] == 0xEB) return "Raspberry Pi";
    if (mac[0] == 0xE8 && mac[1] == 0x31 && mac[2] == 0xCD) return "Linksys/Cisco";
    if (mac[0] == 0x00 && mac[1] == 0x50 && mac[2] == 0x43) return "Marvell (często drony)";

    // --- CHECK BITU LOSOWOŚCI (LAA) ---
    if ((mac[0] & 0x02) == 0x02) return "Mobile/Random";

    return "Unknown";
}


static void wifi_mgmt_event_handler(struct net_mgmt_event_callback *cb,
                                    uint32_t mgmt_event, struct net_if *iface)
{
    // Obsługa surowych danych (tu są dane Remote ID)
    if (mgmt_event == NET_EVENT_WIFI_RAW_SCAN_RESULT) {
        const struct wifi_raw_scan_result *raw = (const struct wifi_raw_scan_result *)cb->info;
        struct DroneDiscoveryData drone_data = {0};
        
        drone_data.rssi = raw->rssi;
        drone_data.protocol_type = 0; 
        drone_data.timestamp = k_uptime_get_32();

        printk("RAW Frame recvd: len %d, RSSI %d\n", raw->frame_length, raw->rssi);

        ODID_UAS_Data UAS_data;
        odid_initUasData(&UAS_data);

        // Przetwarzamy surową ramkę (raw->data) o długości raw->frame_length
        // odid_message_process_pack przeszuka ramkę w poszukiwaniu elementów Remote ID
        if (odid_message_process_pack(&UAS_data, raw->data, raw->frame_length) >= 0) {
            if (UAS_data.LocationValid) {
                snprintf(drone_data.serial_number, sizeof(drone_data.serial_number), "R:%s", UAS_data.BasicID[0].UASID);
                drone_data.latitude = (int32_t)(UAS_data.Location.Latitude * 10000000.0);
                drone_data.longitude = (int32_t)(UAS_data.Location.Longitude * 10000000.0);
                drone_data.altitude_msl = (int32_t)(UAS_data.Location.AltitudeGeo * 100);
                drone_data.speed_horizontal = (int16_t)(UAS_data.Location.SpeedHorizontal * 100);
                drone_data.track_direction = (int16_t)UAS_data.Location.Direction;
                send_to_brain(&drone_data);
            }
        }
    }
    // Obsługa standardowego wyniku (dla wyświetlania urządzeń nie-dronów)
else if (mgmt_event == NET_EVENT_WIFI_SCAN_RESULT) {
        const struct wifi_scan_result *res = (const struct wifi_scan_result *)cb->info;
        struct DroneDiscoveryData drone_data = {0};
        
        const char *vendor = identify_manufacturer(res->mac);
        
        // ZAPISUJEMY: Nazwa + 3 ostatnie bajty MAC dla unikalności
        snprintf(drone_data.serial_number, sizeof(drone_data.serial_number), 
         "%.10s %02X%02X%02X%02X", 
         vendor, res->mac[2], res->mac[3], res->mac[4], res->mac[5]);  
        
        drone_data.rssi = res->rssi;
        drone_data.protocol_type = 0;
        drone_data.timestamp = k_uptime_get_32();
        
        send_to_brain(&drone_data);
    }
    else if (mgmt_event == NET_EVENT_WIFI_SCAN_DONE) {
        k_sleep(K_MSEC(1000));
        struct wifi_scan_params params = { .scan_type = WIFI_SCAN_TYPE_PASSIVE };
        net_mgmt(NET_REQUEST_WIFI_SCAN, iface, &params, sizeof(params));
    }
}

static struct net_mgmt_event_callback wifi_cb;

int start_wifi_sniffer(void) {
    struct net_if *iface = net_if_get_default();
    
    // REJESTRUJEMY TRZY ZDARZENIA: RAW, RESULT i DONE
    net_mgmt_init_event_callback(&wifi_cb, wifi_mgmt_event_handler, 
                                 NET_EVENT_WIFI_SCAN_RESULT | 
                                 NET_EVENT_WIFI_RAW_SCAN_RESULT | 
                                 NET_EVENT_WIFI_SCAN_DONE);
    net_mgmt_add_event_callback(&wifi_cb);
    
    struct wifi_scan_params params = { .scan_type = WIFI_SCAN_TYPE_PASSIVE };
    return net_mgmt(NET_REQUEST_WIFI_SCAN, iface, &params, sizeof(params));
}