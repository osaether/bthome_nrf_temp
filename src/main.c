/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <zephyr/drivers/sensor.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>

#define SERVICE_DATA_LEN        7
#define SERVICE_UUID            0xfcd2		// BTHome service UUID

static const struct device *temp_dev = DEVICE_DT_GET_ANY(nordic_nrf_temp);

#define ADV_PARAM BT_LE_ADV_PARAM(BT_LE_ADV_OPT_USE_IDENTITY, \
					     BT_GAP_ADV_SLOW_INT_MIN, \
					     BT_GAP_ADV_SLOW_INT_MAX, NULL)

static uint8_t service_data[SERVICE_DATA_LEN] = { 
	BT_UUID_16_ENCODE(SERVICE_UUID),
	0x40,
	0x02,	// Temperature
	0x00,
	0x00,
	0x13,
};

static struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR),
	BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
	BT_DATA(BT_DATA_SVC_DATA16, service_data, ARRAY_SIZE(service_data))
};

static void bt_ready(int err)
{
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printk("Bluetooth initialized\n");

	/* Start advertising */
	err = bt_le_adv_start(ADV_PARAM, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}
}

static void init(void)
{
	if (temp_dev == NULL || !device_is_ready(temp_dev)) {
		printk("no temperature found!\n");
		temp_dev = NULL;
	} else {
		printk("temperature device is %p, name is %s\n", temp_dev,
		       temp_dev->name);
	}

	/* Initialize the Bluetooth Subsystem */
	int err = bt_enable(bt_ready);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
	}
}

static uint32_t read_temperature(void)
{
	struct sensor_value temp_value = {0, 0};

	int r = sensor_sample_fetch(temp_dev);
	if (r) {
		printk("sensor_sample_fetch failed return: %d\n", r);
	}

	r = sensor_channel_get(temp_dev, SENSOR_CHAN_DIE_TEMP,
					&temp_value);
	if (r) {
		printk("sensor_channel_get failed return: %d\n", r);
	}
	uint32_t temp = temp_value.val1*100+temp_value.val2/10000;
	return temp;
}

void main(void)
{
	int err;
	uint32_t temp;

	printk("Starting BTHome test\n");

	init();

	for (;;) {
		// Change adv data here:
		// eg. temperature
        temp = read_temperature();
		service_data[4] = temp & 0xff;
		service_data[5] = (temp >> 8) & 0xff;
		err = bt_le_adv_update_data(ad, ARRAY_SIZE(ad), NULL, 0);
		if (err) {
			printk("Failed to update advertising data (err %d)\n", err);
		}
		k_sleep(K_MSEC(BT_GAP_ADV_SLOW_INT_MIN));
	}
}
