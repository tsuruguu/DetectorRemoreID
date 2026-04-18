#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/sys/printk.h>
#include <stdio.h>
#include <string.h>
#include "drone_id_types.hpp"
#include <opendroneid.h>

extern void send_to_brain(struct DroneDiscoveryData *data);

// Musimy ją mieć też tutaj (lub w osobnym pliku .h)
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

struct ble_parse_context {
    struct DroneDiscoveryData *drone_data;
    bool name_found;
    bool rid_found;
};

static bool ble_ad_cb(struct bt_data *data, void *user_data) {
    struct ble_parse_context *ctx = (struct ble_parse_context *)user_data;

    // 1. Nazwa urządzenia (Active Scan Response)
    if ((data->type == BT_DATA_NAME_COMPLETE || data->type == BT_DATA_NAME_SHORTENED)) {
        char temp_name[16];
        size_t name_len = MIN(data->data_len, sizeof(temp_name) - 1);
        memcpy(temp_name, data->data, name_len);
        temp_name[name_len] = '\0';
        
        // Bezpieczne łączenie: Nazwa + to co już jest w serial_number (np. końcówka MAC)
        // Zakładamy, że w serial_number mamy już "Producent MAC" z device_found
        char current_id[21];
        strncpy(current_id, ctx->drone_data->serial_number, sizeof(current_id)-1);

        snprintf(ctx->drone_data->serial_number, 21, "%.10s [%.7s]", temp_name, current_id);
        ctx->name_found = true;
    }

    // 2. Remote ID (ASTM F3411)
    if (data->type == BT_DATA_SVC_DATA16 && data->data_len >= 3) {
        uint16_t svc_uuid = (data->data[1] << 8) | data->data[0];
        if (svc_uuid == 0xFFFA) {
            ODID_UAS_Data UAS_data;
            odid_initUasData(&UAS_data);
            
            if (decodeOpenDroneID(&UAS_data, data->data + 2) != ODID_MESSAGETYPE_INVALID) {
                if (UAS_data.LocationValid) {
                    ctx->drone_data->latitude = (int32_t)(UAS_data.Location.Latitude * 10000000.0);
                    ctx->drone_data->longitude = (int32_t)(UAS_data.Location.Longitude * 10000000.0);
                    ctx->drone_data->altitude_msl = (int32_t)(UAS_data.Location.AltitudeGeo * 100);
                    ctx->drone_data->speed_horizontal = (int16_t)(UAS_data.Location.SpeedHorizontal * 100);
                    ctx->drone_data->track_direction = (int16_t)UAS_data.Location.Direction;
                    
                    // Jeśli to dron, nadpisujemy numerem seryjnym RID (najważniejsza informacja)
                    snprintf(ctx->drone_data->serial_number, 21, "R:%.18s", UAS_data.BasicID[0].UASID);
                    ctx->rid_found = true;
                }
            }
        }
    }
    return true;
}

static void device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t type,
                         struct net_buf_simple *ad)
{
    struct DroneDiscoveryData drone_data = {0};
    struct ble_parse_context ctx = { .drone_data = &drone_data, .name_found = false, .rid_found = false };
    
    const char *vendor = identify_manufacturer(addr->a.val);
    
    // Ustawiamy podstawowy format: "Vendor XX:XX" (bierzemy 2 ostatnie bajty adresu)
    snprintf(drone_data.serial_number, 21, "%s %02X%02X", 
             vendor, addr->a.val[1], addr->a.val[0]);

    bt_data_parse(ad, ble_ad_cb, &ctx);

    drone_data.rssi = rssi;
    drone_data.protocol_type = (type == BT_GAP_ADV_TYPE_ADV_IND) ? 2 : 3; 
    drone_data.timestamp = k_uptime_get_32();
    
    send_to_brain(&drone_data);
}


int start_ble_sniffer(void) {
    // Nie inicjalizuj bt_enable tutaj, skoro robisz to w main.cpp!
    
    struct bt_le_scan_param scan_param = {
        .type       = BT_LE_SCAN_TYPE_ACTIVE,
        // ZMIANA: Dodajemy jawnie obsługę PHY 1M i CODED (drony używają obu)
        .options    = BT_LE_SCAN_OPT_NONE, 
        .interval   = 0x0010, // Szybsze skanowanie (10ms)
        .window     = 0x0010,
    };

    // Upewnij się, że bt_le_scan_start nie zwraca błędu
    int err = bt_le_scan_start(&scan_param, device_found);
    if (err) {
        printk("BŁĄD startu skanowania: %d\n", err);
    }
    return err;
}