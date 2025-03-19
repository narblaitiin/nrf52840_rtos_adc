/*
 * Copyright (c) 2024
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

#include "app_adc.h"

//  ========== globals =====================================================================
int16_t buf;
static const struct adc_dt_spec adc_channel = ADC_DT_SPEC_GET(DT_PATH(zephyr_user));
struct adc_sequence sequence = {
		.buffer = &buf,
		.buffer_size = sizeof(buf), // in bytes
};

//  ========== app_nrf52_adc_init ==========================================================
int8_t app_nrf52_adc_init()
{
    int8_t err;

    if (!adc_is_ready_dt(&adc_channel)) {
		printk("ADC is not ready. error: %d\n", err);
		return 0;
        printk("- found device \"%s\", getting sensor data\n", adc_channel.dev->name);
    }

    // setup ADC channel
    err = adc_channel_setup_dt(&adc_channel);
	if (err < 0) {
		printk("error: %d. could not setup channel\n", err);
		return 0;
	}

    // initializing ADC sequence
    err = adc_sequence_init_dt(&adc_channel, &sequence);
	if (err < 0) {
		printk("error: %d. could not initalize sequnce\n", err);
		return 0;
	}
    return 0;
}

//  ========== app_nrf52_get_adc ===========================================================
int16_t app_nrf52_get_adc()
{
    int16_t val_mv;
    int8_t ret;

    val_mv = (int)buf;
    // reading sample from the ADC
    ret = adc_read(adc_channel.dev, &sequence);
    if (ret < 0 ) {        
	    printk("sensor sample is not up to date. error: %d\n", ret);
	    return 0;
    }

    // battery level received and converted from channel get
    // resolution 12bits: 0 to 4095 (uint16)
    ret = adc_raw_to_millivolts_dt(&adc_channel, &val_mv);
    if (ret < 0) {
			printk("value in mV not available. error: %d\n", ret);
		} else {
			printk("adc: %d mV\n", val_mv);
		}
    return val_mv;
}
