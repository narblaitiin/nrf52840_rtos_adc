/*
 * Copyright (c) 2025
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

//  ========== includes ====================================================================
#include "app_eeprom.h"
#include "app_rtc.h"

//  ========== app_eeprom_init =============================================================
int8_t app_eeprom_init(const struct device *dev)
{
	int8_t ret = 0;
	ssize_t size, page;

	// retrieve the EEPROM device
	dev = DEVICE_DT_GET(SPI_FLASH_DEVICE);
	if (dev == NULL) {
		printk("no eeprom device found. error: %d\n", dev);
		return 0;
	}
	
	// check if the EEPROM device is ready
	if (!device_is_ready(dev)) {
		printk("eeprom device is not ready\n");
		return 0;
	} else {
        printk("- found device \"%s\", proceeding with data operations\n", dev->name);
    }

	// erase the data storage partition
	ret = flash_erase(dev, SPI_FLASH_OFFSET, SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE);
	if (ret != 0){
		printf("MX25R64 flash erase failed. error: %d\n", ret);
	} else {
		printk("MX25R64 flash erase succeeded\n", size);
	}	
	return 0;
}

//  ========== app_eeprom_write ============================================================
int8_t app_eeprom_write(const struct device *dev, int16_t data)
{
	int8_t ret = 0;
	
	// write data to the first page of QSPI flash memory
	ret = flash_write(dev, SPI_FLASH_OFFSET, &data, sizeof(data));

	if (ret!=0) {
		printk("error writing data. error: %d\n", ret);
	} else {
		printk("successfully wrote %zu bytes to address 0x00\n", sizeof(data));
	}

	return 0;
}

//  ========== app_rom_read ================================================================
int16_t app_eeprom_read(const struct device *dev)
{
	int8_t ret = 0;
	int8_t i;
	int16_t data[MAX_RECORDS];

	// Reads data starting from the offset defined by SPI_FLASH_OFFSET into the data array
	ret = flash_read(dev, SPI_FLASH_OFFSET, &data, sizeof(data));
	if (ret) {
		printk("error reading data. error: %d\n", ret);
	} else {
		printk("successfully read %zu bytes from address 0x00\n", sizeof(data));
	}

	return data;
}

//  ======== app_rom_handler ===============================================================
int8_t app_eeprom_handler(const struct device *dev)
{
	int8_t ret = 0;
	int16_t data[MAX_RECORDS] = {0};
	int16_t high, low;
	int32_t timestamp;
	const struct device *rtc;

	// initialize EEPROM device using device tree specification
	dev = DEVICE_DT_GET(SPI_FLASH_DEVICE);

	// initialize DS3231 RTC device via I2C (Pins: SDA -> P0.09, SCL -> P0.0)
    rtc = DEVICE_DT_GET_ONE(maxim_ds3231);

	// retrieve the current timestamp from the RTC device 
	timestamp = app_rtc_get_time(rtc);

	// store the timestamp in the first page of the EEPROM
    // extract the high 16 bits of the timestamp and write them to EEPROM
    high = (int16_t)((timestamp >> 16) & 0xFFFF);
    app_eeprom_write(dev, high);

    // extract the low 16 bits of the timestamp and write them to EEPROM
    low = (int16_t)(timestamp & 0xFFFF);
	app_eeprom_write(dev, low);
	
	// store multiple ADC readings in the first page of the EEPROM
	for (int8_t itr; itr < MAX_RECORDS; itr++) {
		data[itr] = app_nrf52_get_adc();
		app_eeprom_write(dev, data);
	}

	// read back stored data from the EEPROM for verification
	for (int8_t itr = 0; itr < MAX_RECORDS; itr++) {
	 	data[itr] = app_eeprom_read(dev);
	}

	// reconstruct the timestamp from the first two EEPROM entries
	int32_t num = ((int32_t)data[0] << 16) | (int32_t)data[1];
		printk("rd -> timestamp: %d\n", num);

	// print the remaining ADC values read from the EEPROM	
	for (int8_t itr = 2; itr < MAX_RECORDS; itr++) {
		printk("rd -> adc value: %d\n", data[itr]);
   }
	// erase the data storage partition in the EEPROM
	(void)flash_erase(dev, SPI_FLASH_OFFSET, SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE);
	return 0;
}


