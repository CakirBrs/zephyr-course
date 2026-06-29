#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>
#include <stdlib.h>

#include "led_sensor.h"

#define SENSOR_DEV DEVICE_DT_GET_ANY(myco_led_sensor)

static int cmd_sensor_fetch(const struct shell *sh, size_t argc, char **argv)
{
	const struct device *dev = SENSOR_DEV;

	if (dev == NULL || !device_is_ready(dev)) {
		shell_error(sh, "sensor not ready");
		return -ENODEV;
	}

	int ret = sensor_sample_fetch(dev);

	if (ret < 0) {
		shell_error(sh, "fetch failed: %d", ret);
		return ret;
	}
	shell_print(sh, "fetch ok (LED on)");
	return 0;
}

static int cmd_sensor_read(const struct shell *sh, size_t argc, char **argv)
{
	const struct device *dev = SENSOR_DEV;
	struct sensor_value val;

	if (dev == NULL || !device_is_ready(dev)) {
		shell_error(sh, "sensor not ready");
		return -ENODEV;
	}

	int ret = sensor_channel_get(dev, SENSOR_CHAN_ALL, &val);

	if (ret < 0) {
		shell_error(sh, "read failed: %d", ret);
		return ret;
	}
	shell_print(sh, "read: %d.%06d (LED off)", val.val1, val.val2);
	return 0;
}

static int cmd_sensor_info(const struct shell *sh, size_t argc, char **argv)
{
	const struct device *dev = SENSOR_DEV;

	if (dev == NULL) {
		shell_error(sh, "no sensor device found");
		return -ENODEV;
	}
	shell_print(sh, "name : %s", dev->name);
	shell_print(sh, "ready: %s", device_is_ready(dev) ? "yes" : "no");
	return 0;
}

/* Alt-komutlar */
SHELL_STATIC_SUBCMD_SET_CREATE(sensor_cmds,
	SHELL_CMD(fetch, NULL, "Call sensor_sample_fetch (LED on)", cmd_sensor_fetch),
	SHELL_CMD(read,  NULL, "Call sensor_channel_get (LED off)", cmd_sensor_read),
	SHELL_CMD(info,  NULL, "Print device name and ready state", cmd_sensor_info),
	SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensor, &sensor_cmds, "LED-sensor driver commands", NULL);