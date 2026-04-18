/* common/include/drone_id_types.hpp */
#ifndef DRONE_ID_TYPES_HPP
#define DRONE_ID_TYPES_HPP

#include <zephyr/types.h>

/**
 * @brief Struktura przechowująca skonsolidowane dane Remote ID drona.
 * Używamy __packed, aby zapewnić identyczny układ bajtów na obu płytkach.
 */
struct __packed DroneDiscoveryData {
    char serial_number[1];    // Unikalny ID drona (ASTM F3411) 
    int32_t latitude;          // Szerokość geograficzna (skalowana: stopnie * 10^7)
    int32_t longitude;         // Długość geograficzna
    int32_t altitude_msl;      // Wysokość nad poziomem morza (cm)
    int16_t speed_horizontal;  // Prędkość pozioma (cm/s)
    int16_t track_direction;   // Kierunek lotu (0-359 stopni)
    int8_t rssi;               // Siła sygnału odebrana przez nRF7002 (dBm) [2, 3]
    uint8_t protocol_type;     // 0: Wi-Fi Beacon, 1: Wi-Fi NaN, 2: BLE [4, 5]
    uint32_t timestamp;        // Czas odebrania ramki
};

#endif // DRONE_ID_TYPES_HPP