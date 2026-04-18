/* nrf54_brain/src/ai_engine.hpp */
#ifndef AI_ENGINE_HPP
#define AI_ENGINE_HPP

#include <edge-impulse-sdk/classifier/ei_run_classifier.h>

class AiEngine {
public:
    AiEngine() { /* Inicjalizacja ewentualnych buforów */ }

    /**
     * @brief Przewiduje odległość drona na podstawie siły sygnału i parametrów pogodowych.
     * @param rssi Siła sygnału w dBm
     * @param temp Temperatura z BME280
     * @param hum Wilgotność z BME280
     */
    float predict_distance(int8_t rssi, float temp, float hum) {
        // Przygotowanie danych wejściowych (zgodnie z kolejnością w Edge Impulse)
        float features = { (float)rssi, temp, hum };
        
        signal_t signal;
        numpy::signal_from_buffer(features, sizeof(features) / sizeof(features), &signal);

        ei_impulse_result_t result = { 0 };
        EI_IMPULSE_ERROR err = run_classifier(&signal, &result, false);

        if (err!= EI_IMPULSE_OK) {
            return -1.0f;
        }

        // Jeśli używasz modelu regresji, wynik jest w result.classification.value
        return result.classification.value;
    }
};

#endif