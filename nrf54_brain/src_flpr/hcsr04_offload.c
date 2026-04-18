/* nrf54_brain/src_flpr/hcsr04_offload.c */
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define TRIG_PIN 4 // P1.04 (przykładowo)
#define ECHO_PIN 5 // P1.05

void main(void) {
    // Niskopoziomowa pętla RISC-V
    // FLPR ma bezpośredni dostęp do timerów GRTC nRF54L15
    while(1) {
        // 1. Wysyłanie impulsu 10us na TRIG
        // 2. Czekanie na narastające zbocze na ECHO
        // 3. Start precyzyjnego licznika
        // 4. Stop na zboczu opadającym
        // 5. Przesłanie wyniku do M33 przez IPC/Shared RAM
    }
}