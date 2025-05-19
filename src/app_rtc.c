/*
 * Copyright (c) 2025
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

//  ========== includes ====================================================================
#include "app_rtc.h"

//  ========== app_rtc_set_time ============================================================
// int64_t app_rtc_set_time(const struct device *i2c_dev, const struct tm *date_time)
// {
//     uint32_t counter_value = 0;

//     int8_t ret = counter_get_value(i2c_dev, &counter_value);
//     if (ret) {
//         printk("error setting RTC time. error: %d\n", ret);
//         return 0;
//     }

//     // convert struct tm to Unix timestamp
//     time_t timestamp = timegm(date_time);

//     /* Update the offset to align the current RTC value with the new time */
//     int64_t time_offset = timestamp - counter_value;
//     printk("RTC time set successfully. offset: %lld seconds\n");

//     return time_offset;
// }

//  ========== app_rtc_get_time ============================================================
int32_t app_rtc_get_time(const struct device *i2c_dev)
{
    int8_t ret = 0;
	int32_t timestamp = 0;
	struct rtc_time time = {0};

	// convert rtc_time to Unix timestamp (seconds since 1970)
    struct tm t = {
        .tm_year = time.tm_year,
        .tm_mon  = time.tm_mon,
        .tm_mday = time.tm_mday,
        .tm_hour = time.tm_hour,
        .tm_min  = time.tm_min,
        .tm_sec  = time.tm_sec
    };

    ret = rtc_get_time(i2c_dev, &time);
	if (ret < 0) {
		printk("cannot read date time: %d\n", ret);
		return ret;
	}

	// convert to Unix timestamp
	timestamp = (int32_t)mktime(&t);
	printk("timestamp in unix: %d\n", timestamp);

    return timestamp;
}

//  ========== app_rtc_init ================================================================
const struct device *app_rtc_init(void)
{
 //   const struct device *rtc_dev = DEVICE_DT_GET_ONE(maxim_ds3231);
    const struct device *rtc_dev = DEVICE_DT_GET_ONE(maxim_ds3231);
    if (!rtc_dev) {
        printk("no RTC device found");
        return NULL;
    }

    if (!device_is_ready(rtc_dev)) {
        printk("RTC device is not ready");
        return NULL;
    }

    printk("RTC device \"%s\" initialized successfully", rtc_dev->name);
    return rtc_dev;
}