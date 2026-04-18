/* common/include/drone_id_types.hpp */
#ifndef DRONE_ID_TYPES_HPP
#define DRONE_ID_TYPES_HPP

#include <zephyr/types.h>

/**
 * @brief Struktura przechowująca skonsolidowane dane Remote ID drona.
 */
struct __packed DroneDiscoveryData {
    // Standard ASTM F3411 definiuje serial jako max 20 znaków + terminator
    char serial_number[21];    
    
    int32_t latitude;          // Szerokość (stopnie * 10^7)
    int32_t longitude;         // Długość (stopnie * 10^7)
    int32_t altitude_msl;      // Wysokość MSL (cm)
    int16_t speed_horizontal;  // Prędkość (cm/s)
    int16_t track_direction;   // Kierunek (0-359 stopni)
    
    int8_t rssi;               // Siła sygnału (dBm)
    
    // Typ protokołu: 0: WiFi_Beacon, 1: WiFi_NAN, 2: BLE_Legacy, 3: BLE_Extended
    uint8_t protocol_type;     
    
    uint32_t timestamp;        // Czas systemowy odebrania ramki (ms)
};

#endif // DRONE_ID_TYPES_HPP