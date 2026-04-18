#pragma once
#include "../../common/include/drone_id_types.hpp"

enum class ThreatLevel {
    LOW,        // Normalny lot
    MEDIUM,     // Nietypowe zachowanie / Anomalia pogodowa
    HIGH,       // Podejrzenie spoofingu / Kurs kolizyjny
    CRITICAL    // Naruszenie strefy
};

class AiEngine {
public:
    AiEngine() = default;

    bool load_model(); // Ładowanie wag modelu TinyML

    // GŁÓWNA FUNKCJA: Łączy dane z Remote ID oraz dane pogodowe
    ThreatLevel analyze_threat(const DroneData& drone, const EnvData& env) {
        // 1. Normalizacja danych (Input scaling)
        // 2. Inference (Uruchomienie sieci neuronowej)
        // 3. Interpretacja wyników
        return ThreatLevel::LOW;
    }

private:
    // Tu będą wskaźniki do interpretera TFLite Micro
};