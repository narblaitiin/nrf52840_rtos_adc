/*
 * Copyright (c) 2025
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

//  ========== includes ====================================================================
#include "app_eeprom.h"
#include "app_adc.h"
#include "app_rtc.h"

//  ========== interrupt sub-routine =======================================================
void geo_work_handler(struct k_work *work_geo)
{
// 	const struct device *rom_dev = DEVICE_DT_GET(SPI_FLASH_DEVICE);

 	// printk("ADC handler called\n");
 	// app_eeprom_handler(rom_dev);

 	// printk("test only sensor connected on ADC P0.03\n");
 	// int16_t value = app_nrf52_get_adc();
 	// printk("return velocity: %d mV\n", value);

//	printk("test only internal RTC device\n");
	uint64_t timestamp =  app_rtc_get_time();  // Pass the address of 'timestamp'
	printk("timestamp in ms: %llu\n", timestamp);
}
K_WORK_DEFINE(geo_work, geo_work_handler);

void geo_timer_handler(struct k_timer *geo_dum)
{
	k_work_submit(&geo_work);
}
K_TIMER_DEFINE(geo_timer, geo_timer_handler, NULL);

// ========== main ========================================================================
int8_t main(void)
{
	// initialize DS3231 RTC device via I2C (Pins: SDA -> P0.09, SCL -> P0.0)
	const struct device *rtc_dev = app_rtc_init();
    if (!rtc_dev) {
        printk("failed to initialize RTC device\n");
        return 0;
    }

	sync_uptime_with_rtc(rtc_dev);

	// initialize ADC device
	// int8_t ret = app_nrf52_adc_init();
	// if (ret != 1) {
	// 	printk("failed to initialize ADC device\n");
	// 	return 0;
	// }

	// retrieve the EEPROM device
	// const struct device *flash_dev = DEVICE_DT_GET(SPI_FLASH_DEVICE);
	// ret = app_eeprom_init(flash_dev);
	// if (ret != 1) {
	// 	printk("failed to initialize QSPI Flash device\n");
	// 	return 0;
	// }

	printk("ADC nRF52 and RTC DS3231 Example\n");

	// start the timer to trigger the interrupt subroutine every 30 seconds
	k_timer_start(&geo_timer, K_NO_WAIT, K_MSEC(1000));

	return 0;
}