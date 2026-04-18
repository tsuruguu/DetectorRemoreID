#ifndef BME280_HANDLER_HPP
#define BME280_HANDLER_HPP

#include <zephyr/drivers/sensor.h>

class BmeHandler {
public:
    struct EnvData {
        float temperature;
        float humidity;
        float pressure;
    };

    EnvData get_reading(const struct device *dev) {
        struct sensor_value t, h, p;
        sensor_sample_fetch(dev);
        sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &t);
        sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY, &h);
        sensor_channel_get(dev, SENSOR_CHAN_PRESS, &p);
        return {(float)sensor_value_to_double(&t), (float)sensor_value_to_double(&h), (float)sensor_value_to_double(&p)};
    }
};

#endif