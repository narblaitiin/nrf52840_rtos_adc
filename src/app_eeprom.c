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

	// getting EEPROM size
	dev = DEVICE_DT_GET(SPI_FLASH_DEVICE);
	if (dev == NULL) {
		printk("no eeprom device found. error: %d\n", dev);
		return 0;
	}
	
	if (!device_is_ready(dev)) {
		printk("eeprom is not ready\n");
		return 0;
	} else {
        printk("- found device \"%s\", writing/reading data\n", dev->name);
    }

	// getting the total number of flash pages in bytes
	page = flash_get_page_count(dev);
	printk("number of pages: %zu\n", page);

	// cleaning data storage partition
	ret = flash_erase(dev, SPI_FLASH_OFFSET, SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE);
	if (ret != 0){
		printf("MX25R64 flash erase failed. error: %d ", ret);
	} else {
		printk("MX25R64 flash erase succeeded", size);
	}	
	return 0;
}

//  ========== app_eeprom_write ============================================================
int8_t app_eeprom_write(const struct device *dev, int16_t data)
{
	int8_t ret = 0;
	
	// writing data in the first page of QSPI flah memory
	ret = flash_write(dev, SPI_FLASH_OFFSET, &data, sizeof(data));

	if (ret!=0) {
		printk("error writing data. error: %d\n", ret);
	} else {
		printk("wrote %zu bytes to address 0x00\n", sizeof(data));
	}

	// printing data
	printk("wrt -> rom val: %d\n", data);
	return 0;
}

//  ========== app_rom_read ================================================================
int16_t app_eeprom_read(const struct device *dev)
{
	int8_t ret = 0;
	int8_t i;
	int16_t data[MAX_RECORDS];

	// reading the first page
	ret = flash_read(dev, SPI_FLASH_OFFSET, &data, sizeof(data));
	if (ret) {
		printk("error reading data. error: %d\n", ret);
	} else {
		printk("read %zu bytes from address 0x00\n", sizeof(data));
	}

	for (int8_t i = 0; i < MAX_RECORDS; i++) {
		printk("rd -> rom val: %d\n", data[i]);
	}
	return data;
}

//  ======== app_rom_handler =======================================
int8_t app_eeprom_handler(const struct device *dev)
{
	int8_t ret = 0;
	int16_t data[MAX_RECORDS];
	int16_t high, low;
	int32_t timestamp;
	const struct device *rtc;

	// getting eeprom device
	dev = DEVICE_DT_GET(SPI_FLASH_DEVICE);

	// getting ds3231 rtc i2c device (SDA: P0.09, SCL:P0.0)
    rtc = DEVICE_DT_GET_ONE(maxim_ds3231);

	// getting time 
	timestamp = app_rtc_get_time(rtc);

	// putting timestamp at the beginning of the fisrt page for this test
	// extract the high 16 bits and write to eeprom
    high = (int16_t)((timestamp >> 16) & 0xFFFF);
    app_eeprom_write(dev, high);

    // extract the low 16 bits
    low = (int16_t)(timestamp & 0xFFFF);
	app_eeprom_write(dev, low);
	
	// putting n structures in fisrt page for this test
	for (int8_t itr; itr < MAX_RECORDS; itr++) {
		data[itr] = app_nrf52_get_adc();
		app_eeprom_write(dev, data);
	}

	// reading stored data
	for (int8_t itr = 0; itr < MAX_RECORDS; itr++) {
	 	data[itr] = app_eeprom_read(dev);
	}

	int32_t num = ((int32_t)data[0] << 16) | (int32_t)data[1];
		printk("rd -> timestamp: %d\n", num);

	for (int8_t itr = 2; itr < MAX_RECORDS; itr++) {
		printk("rd -> adc value: %d\n", data[itr]);
   }
	// cleaning data storage partition
	(void)flash_erase(dev, SPI_FLASH_OFFSET, SPI_FLASH_SECTOR_SIZE*SPI_FLASH_PAGE_SIZE);
	return 0;
}


