/*
 * Copyright (c) 2025
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

//  ========== includes ====================================================================
#include "app_adc.h"

//  ========== globals =====================================================================
// ADC buffer to store raw ADC readings
int16_t buf;

// ADC channel configuration obtained from the device tree
static const struct adc_dt_spec adc_channel = ADC_DT_SPEC_GET(DT_PATH(zephyr_user));

// ADC sequence configuration to specify the ADC operation
static struct adc_sequence sequence = {
    .channels = 1,
	.buffer = &buf,
	.buffer_size = sizeof(buf),
};

//  ========== app_nrf52_adc_init ==========================================================
int8_t app_nrf52_adc_init()
{
    int8_t ret = 0;

    // verify if the ADC is ready for operation
    if (!adc_is_ready_dt(&adc_channel)) {
		printk("ADC is not ready. error: %d\n", ret);
		return 0;
	} else {
        printk("- found device \"%s\", getting vbat data\n", adc_channel.dev->name);
    }

    // configure the ADC channel settings
    ret = adc_channel_setup_dt(&adc_channel);
	if (ret < 0) {
		printk("failed to set up ADC channel. error: %d\n", ret);
		return 0;
	}

    // Initialize the ADC sequence for continuous or single readings
    ret = adc_sequence_init_dt(&adc_channel, &sequence);
	if (ret < 0) {
		printk("failed to initialize ADC sequence. error: %d\n", ret);
		return 0;
	}
    return 0;
}

//  ========== app_nrf52_get_adc ===========================================================
int16_t app_nrf52_get_adc()
{
    int16_t velocity;       // converted voltage value in mV
    int8_t ret;
    
    // trigger an ADC read and store the result in the configured buffer
    ret = adc_read(adc_channel.dev, &sequence);
    if (ret < 0) {        
	    printk("failed to read raw ADC value. Error: %d\n", ret);
	    return 0;
    }

    printk("raw adc value: %d\n", buf);

    // convert the raw ADC reading into a voltage value (in millivolts)
    velocity = (buf * ADC_REFERENCE_VOLTAGE) / ADC_RESOLUTION;
    printk("velocity: %d mV\n", velocity);

    return (int16_t)velocity;
}
