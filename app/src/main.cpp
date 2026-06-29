#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/sensor.h>
#include "led_sensor.h"
//#define SLEEP_TIME_MS 1000

/* The devicetree node identifier for the "led0" alias. */
//#define LED_NODE DT_ALIAS(led0)
//#define APP_LED_NODE DT_ALIAS(app_led)


//static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);
//static const struct gpio_dt_spec heartbeat = GPIO_DT_SPEC_GET(APP_LED_NODE, gpios);

//LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    /*
    bool led_state = true;

    if (!gpio_is_ready_dt(&heartbeat)) return 0;

    if (gpio_pin_configure_dt(&heartbeat, GPIO_OUTPUT_ACTIVE) < 0) return 0;

    LOG_INF("Heartbeat period: %d ms", CONFIG_APP_HEARTBEAT_PERIOD_MS);
*/
    const struct device *sensor = DEVICE_DT_GET_ANY(myco_led_sensor);
    if (sensor == NULL || !device_is_ready(sensor)) {
		return 0;
	}

	struct sensor_value val;
    led_sensor_set_blink_count(sensor, 10);
    while (1) {
        //if (gpio_pin_toggle_dt(&heartbeat) < 0) return 0;

        //led_state = !led_state;
        //LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
        //k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
        sensor_sample_fetch(sensor); //LED ON
        k_msleep(500);
		sensor_channel_get(sensor, SENSOR_CHAN_ALL, &val); //LED OFF
		k_msleep(500);
    }
    return 0;
}
