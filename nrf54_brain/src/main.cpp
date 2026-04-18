/* nrf54_brain/src/main.cpp */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include "ai_engine.hpp"
#include "bme280_handler.hpp"
#include "ui_manager.hpp"
#include "drone_id_types.hpp"

// Deklaracja funkcji pobierającej dane, zdefiniowanej w data_aggregator.cpp
extern int data_aggregator_get_next(DroneDiscoveryData *data);

void main(void) {
    const struct device *const bme_dev = DEVICE_DT_GET_ANY(bosch_bme280);
    AiEngine ai;
    UiManager ui;

    // Startowy komunikat diagnostyczny
    printk("Remote ID Brain started on nRF54L15\n");

    while (1) {
        struct sensor_value temp, press, hum;
        
        // Pobieranie danych pogodowych dla korekcji RSSI [1, 2]
        sensor_sample_fetch(bme_dev);
        sensor_channel_get(bme_dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);
        sensor_channel_get(bme_dev, SENSOR_CHAN_HUMIDITY, &hum);

        // Odbiór danych Remote ID przesłanych z płytki nRF7002
        DroneDiscoveryData drone;
        if (data_aggregator_get_next(&drone) == 0) {
            // Wnioskowanie AI: RSSI + Temperatura + Wilgotność -> Estymowana odległość [3, 4]
            float est_distance = ai.predict_distance(drone.rssi, (float)temp.val1, (float)hum.val1);
            
            // --- AKTUALIZACJA DLA GATEWAYA ---
            // Wysyłamy dane w formacie JSON, który skrypt Python rozpozna i prześle do panelu.
            // Wykorzystujemy pola zdekodowane przez bibliotekę opendroneid-core-c.
            printk("{\"drone_id\": \"%s\", \"rssi\": %d, \"dist\": %.2f, \"lat\": %d, \"lon\": %d}\n", 
                   drone.serial_number, drone.rssi, est_distance, drone.latitude, drone.longitude);
            
            // Lokalna wizualizacja na wyświetlaczu HC595
            ui.update_display(est_distance, drone.protocol_type);
            
            if (est_distance < 10.0f) {
                ui.alert_proximity();
            }
        }
        
        // Krótkie uśpienie, aby nie blokować procesora [6]
        k_msleep(100);
    }
}