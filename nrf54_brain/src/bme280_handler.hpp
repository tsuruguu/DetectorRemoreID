#pragma once
#include "../../common/include/drone_id_types.hpp"

class Bme280Handler {
public:
    Bme280Handler() = default;

    // Inicjalizacja sensora przez I2C (Zephyr Device Tree)
    bool init();

    // Pobiera surowe dane i aktualizuje EnvData
    void update();

    // Główna funkcja "AIoT" - oblicza wpływ pogody na propagację fal
    float calculate_rf_refraction_index() {
        // Tu znajdzie się implementacja wzoru na współczynnik N (refraktywność)
        // N = (77.6 / T) * (P + 4810 * e / T)
        return 0.0f; 
    }

    EnvData get_latest_env_data() { return last_data; }

private:
    EnvData last_data;
    const struct device *i2c_dev;
};