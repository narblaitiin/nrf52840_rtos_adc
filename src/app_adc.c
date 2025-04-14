/*
 * Copyright (c) 2025
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

//  ========== includes ====================================================================
#include "app_adc.h"

//  ========== globals =====================================================================
int16_t buf;
static const struct adc_dt_spec adc_channel = ADC_DT_SPEC_GET(DT_PATH(zephyr_user));
static struct adc_sequence sequence = {
    .channels = 1,
	.buffer = &buf,
	.buffer_size = sizeof(buf),
};

//  ========== app_nrf52_adc_init ==========================================================
int8_t app_nrf52_adc_init()
{
    int8_t ret = 0;

    if (!adc_is_ready_dt(&adc_channel)) {
		printk("ADC is not ready. error: %d\n", ret);
		return 0;
	} else {
        printk("- found device \"%s\", getting vbat data\n", adc_channel.dev->name);
    }

    // setup ADC channel
    ret = adc_channel_setup_dt(&adc_channel);
	if (ret < 0) {
		printk("could not setup channel. error: %d\n", ret);
		return 0;
	}

    // initializing ADC sequence
    ret = adc_sequence_init_dt(&adc_channel, &sequence);
	if (ret < 0) {
		printk("could not initalize sequnce. error: %d\n", ret);
		return 0;
	}
    return 0;
}

//  ========== app_nrf52_get_adc ===========================================================
int16_t app_nrf52_get_adc()
{
    int16_t velocity;
    int8_t ret;
    
    // read sample from the ADC
    ret = adc_read(adc_channel.dev, &sequence);
    if (ret < 0) {        
	    printk("raw adc value is not up to date. error: %d\n", ret);
	    return 0;
    }

    printk("raw adc value: %d\n", buf);

    // convert ADC reading to voltage
    velocity = (buf * ADC_REFERENCE_VOLTAGE) / ADC_RESOLUTION;
    printk("velocity: %d mV\n", velocity);

    return (int16_t)velocity;
}
