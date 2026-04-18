/* nrf54_brain/src/main.cpp */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include "ai_engine.hpp"
#include "bme280_handler.hpp"
#include "ui_manager.hpp"
#include "drone_id_types.hpp"

void main(void) {
    const struct device *const bme_dev = DEVICE_DT_GET_ANY(bosch_bme280);
    AiEngine ai;
    UiManager ui;

    printk("Remote ID Brain started on nRF54L15\n");

    while (1) {
        struct sensor_value temp, press, hum;
        sensor_sample_fetch(bme_dev);
        sensor_channel_get(bme_dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);
        sensor_channel_get(bme_dev, SENSOR_CHAN_HUMIDITY, &hum);

        // Pobieranie danych z kolejki agresora danych (z nRF7002)
        DroneDiscoveryData drone;
        if (data_aggregator_get_next(&drone) == 0) {
            // Wnioskowanie AI: RSSI + Warunki pogodowe -> Dokładna odległość [3, 4]
            float est_distance = ai.predict_distance(drone.rssi, temp.val1, hum.val1);
            
            // Weryfikacja pozycji (Cross-check AI vs GPS z Remote ID)
            ui.update_display(est_distance, drone.protocol_type);
            
            if (est_distance < 10.0f) {
                ui.alert_proximity();
            }
        }
        k_msleep(100);
    }
}