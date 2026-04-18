#pragma once
#include <stdint.h>

// Struktura przechowująca dane zdekodowane z Remote ID (ASTM F3411)
struct DroneData {
    char operator_id[20];
    float latitude;
    float longitude;
    float altitude;
    float speed_horizontal; 
    float speed_vertical;
    uint16_t heading;
    int16_t rssi; 
};

// Struktura dla danych środowiskowych do korekcji RF
struct EnvData {
    float temperature;
    float humidity;
    float pressure;
    float air_density;
    float rf_correction_factor; 
};