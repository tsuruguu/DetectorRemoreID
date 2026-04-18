/* common/include/inter_board_protocol.h */
#ifndef INTER_BOARD_PROTOCOL_H
#define INTER_BOARD_PROTOCOL_H

#include <stdint.h>

#define PROTOCOL_MAGIC_BYTE 0xAA  // Bajt startu ramki
#define MSG_TYPE_DRONE_DATA 0x01  // Typ wiadomości: dane o dronie
#define MSG_TYPE_HEARTBEAT  0x02  // Typ wiadomości: status połączenia

/**
 * @brief Nagłówek ramki komunikacyjnej UART
 */
struct __packed IpcHeader {
    uint8_t magic;
    uint8_t type;
    uint16_t length;
};

/**
 * @brief Oblicza prostą sumę kontrolną XOR dla pakietu.
 */
static inline uint8_t calculate_checksum(const uint8_t *data, uint16_t len) {
    uint8_t checksum = 0;
    for (uint16_t i = 0; i < len; i++) {
        checksum ^= data[i];
    }
    return checksum;
}

#endif // INTER_BOARD_PROTOCOL_H