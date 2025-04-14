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
// void geo_work_handler(struct k_work *work_geo)
// {
// 	const struct device *rom_dev;

// 	printk("ADC handler called\n");
// 	app_eeprom_handler(rom_dev);
// }
// K_WORK_DEFINE(geo_work, geo_work_handler);

// void geo_timer_handler(struct k_timer *geo_dum)
// {
// 	k_work_submit(&geo_work);
// }
// K_TIMER_DEFINE(geo_timer, geo_timer_handler, NULL);

//  ========== main ========================================================================
int8_t main(void)
{
	const struct device *rom_dev;
	const struct device *rtc_dev;

	// setup eeprom device
//	app_eeprom_init(rom_dev);
	app_rtc_init(rtc_dev);
	app_nrf52_adc_init();

	printk("ADC nRF52 and RTC DS3231 Example\nBoard: %s\n", CONFIG_BOARD);

	// beginning of interrupt subroutine
//	k_timer_start(&geo_timer, K_NO_WAIT, K_MSEC(30000));

	while (1) {
	//	int32_t time = app_rtc_get_time (rtc_dev);
	//	k_msleep(1000);
		int16_t value = app_nrf52_get_adc();
		k_msleep(5000);
	}
	
	return 0;
}