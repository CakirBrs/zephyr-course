#define DT_DRV_COMPAT myco_led_sensor

#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include "led_sensor.h"

struct led_sensor_config {
	struct gpio_dt_spec led;        
};

struct led_sensor_data {
	bool led_on;      
    uint32_t blink_count;             
};

static int led_sensor_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
	const struct led_sensor_config *cfg = dev->config;
	struct led_sensor_data *data = dev->data;

	gpio_pin_set_dt(&cfg->led, 1);  // ON
	data->led_on = true;
	return 0;
}

static int led_sensor_channel_get(const struct device *dev, enum sensor_channel chan,
				  struct sensor_value *val)
{
	const struct led_sensor_config *cfg = dev->config;
	struct led_sensor_data *data = dev->data;

	gpio_pin_set_dt(&cfg->led, 0);  // OFF
	data->led_on = false;

	if (val != NULL) {
		val->val1 = data->led_on ? 1 : 0;
		val->val2 = 0;
	}
	return 0;
}

static const struct sensor_driver_api led_sensor_api = {
	.sample_fetch = led_sensor_sample_fetch,
	.channel_get  = led_sensor_channel_get,
};

static int led_sensor_init(const struct device *dev)
{
	const struct led_sensor_config *cfg = dev->config;

	if (!gpio_is_ready_dt(&cfg->led)) {
		return -ENODEV;
	}
	return gpio_pin_configure_dt(&cfg->led, GPIO_OUTPUT_INACTIVE);
}

#define LED_SENSOR_INIT(inst)							\
	static struct led_sensor_data led_sensor_data_##inst;			\
	static const struct led_sensor_config led_sensor_config_##inst = {	\
		.led = GPIO_DT_SPEC_INST_GET(inst, gpios),			\
	};									\
	DEVICE_DT_INST_DEFINE(inst, led_sensor_init, NULL,			\
			      &led_sensor_data_##inst,				\
			      &led_sensor_config_##inst,			\
			      POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,		\
			      &led_sensor_api);

DT_INST_FOREACH_STATUS_OKAY(LED_SENSOR_INIT)

int led_sensor_set_blink_count(const struct device *dev, uint32_t count)
{
	struct led_sensor_data *data = dev->data;
	data->blink_count = count;
	return 0;
}

uint32_t led_sensor_get_blink_count(const struct device *dev)
{
	struct led_sensor_data *data = dev->data;
	return data->blink_count;
}