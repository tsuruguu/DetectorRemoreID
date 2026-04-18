#pragma once
#include "../../common/include/drone_id_types.hpp"

class UiManager {
public:
    // Aktualizacja wyświetlacza 4-cyfrowego (np. odległość do drona)
    void update_display(float value);

    // Alarm dźwiękowy zależny od poziomu zagrożenia z AI
    void play_alarm(ThreatLevel level);

    // Odczyt pozycji joysticka do nawigacji po menu
    void poll_joystick();

private:
    // Piny sterujące HC595 i PWM dla buzzera
};