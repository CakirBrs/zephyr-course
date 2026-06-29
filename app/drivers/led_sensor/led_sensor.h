#ifndef LED_SENSOR_H
#define LED_SENSOR_H

#include <zephyr/device.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int led_sensor_set_blink_count(const struct device *dev, uint32_t count);
uint32_t led_sensor_get_blink_count(const struct device *dev);

#ifdef __cplusplus
}
#endif

#endif