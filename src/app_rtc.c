/*
 * Copyright (c) 2025
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

//  ========== includes ====================================================================
#include "app_rtc.h"

//  ========== app_rtc_init ================================================================
int8_t app_rtc_init(const struct device *dev)
{
    int8_t ret = 0;
    
    // get DS3231 RTC device (I2C interface: SDA on P0.24, SCL on P0.25)
    dev = DEVICE_DT_GET_ONE(maxim_ds3231);
    if (dev == 0) {
        printk("error: no RTC device found\n");
		return 0;
	}

    // check if the device is ready for use
    if (!device_is_ready(dev)) {
		printk("error: rtc is not ready\n");
		return 0;
	} else {
        printk("- found device \"%s\", getting RTC data\n", dev->name);
    }
    return 0;
}

//  ========== app_rtc_get_time ============================================================
int32_t app_rtc_get_time (const struct device *dev)
{
    int8_t ret = 0;
	int32_t timestamp = 0;
	struct rtc_time time = {0};

	// convert rtc_time structure to tm structure for Unix timestamp conversion
    struct tm t = {
        .tm_year = time.tm_year - 1900,
        .tm_mon  = time.tm_mon - 1,
        .tm_mday = time.tm_mday,
        .tm_hour = time.tm_hour,
        .tm_min  = time.tm_min,
        .tm_sec  = time.tm_sec
    };

    // get DS3231 RTC device (I2C interface: SDA on P0.24, SCL on P0.25)
    dev = DEVICE_DT_GET_ONE(maxim_ds3231);

    // read the current date and time from the RTC
    ret = rtc_get_time(dev, &time);
	if (ret < 0) {
		printk("cannot read date time: %d\n", ret);
		return ret;
	}

    // debug: print the retrieved RTC time for verification
    printk("RTC time: year=%d, month=%d, day=%d, hour=%d, min=%d, sec=%d\n",
           time.tm_year, time.tm_mon, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);

    // convert the RTC time to a Unix timestamp
    time_t unix_timestamp = mktime(&t);
    if (unix_timestamp == -1) {
        printk("failed to convert to Unix timestamp\n");
        return -EINVAL;
    }

    // store the Unix timestamp as an int32_t value
	timestamp = (int32_t)mktime(&t);
	printk("timestamp in unix: %d\n", timestamp);

    return timestamp;
}