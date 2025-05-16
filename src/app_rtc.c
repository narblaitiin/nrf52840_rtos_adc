/*
 * Copyright (c) 2025
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

//  ========== includes ====================================================================
#include "app_rtc.h"

//  ========== app_rtc_set_time ============================================================
int64_t app_rtc_set_time(const struct device *i2c_dev, const struct tm *date_time)
{
    uint32_t counter_value = 0;

    int8_t ret = counter_get_value(i2c_dev, &counter_value);
    if (ret) {
        printk("error setting RTC time. error: %d\n", ret);
        return 0;
    }

    // convert struct tm to Unix timestamp
    time_t timestamp = timegm(date_time);

    /* Update the offset to align the current RTC value with the new time */
    int64_t time_offset = timestamp - counter_value;
    printk("RTC time set successfully. offset: %lld seconds\n");

    return time_offset;
}

//  ========== app_rtc_get_time ============================================================
int32_t app_rtc_get_time(const struct device *i2c_dev, int64_t offset)
{
    uint32_t counter_value;

    // get the current counter value in seconds
    int8_t ret = counter_get_value(i2c_dev, &counter_value);
    if (ret) {
        printk("failed to get RTC time. error: %d\n", ret);
        return 0;
    }

    // adjust the counter value with the offset
    time_t unix_timestamp = counter_value + offset;
    
    // convert Unix timestamp to struct tm
    struct tm *current_time = gmtime(&unix_timestamp);
    
    printk("current date and time: %04d-%02d-%02d %02d:%02d:%02d\n",
            current_time->tm_year + 1900, current_time->tm_mon + 1, current_time->tm_mday,
            current_time->tm_hour, current_time->tm_min, current_time->tm_sec);

    int32_t timestamp = (int32_t)mktime(current_time);
    printk("unix timestamp: %d\n", timestamp);

    return timestamp;
}

//  ========== app_rtc_init ================================================================
const struct device *app_rtc_init(void)
{
 //   const struct device *rtc_dev = DEVICE_DT_GET_ONE(maxim_ds3231);
    const struct device *rtc_dev = DEVICE_DT_GET(DT_NODELABEL(rtc0));
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