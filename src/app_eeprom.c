/*
 * Copyright (c) 2025
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

//  ========== includes ====================================================================
#include "app_eeprom.h"
#include "app_rtc.h"

//  ========== serialize_uint64_to_bytes ===================================================
// serialize a uint64_t to bytes
static void serialize_uint64_to_bytes(uint64_t value, uint8_t *buffer) {
    for (int i = 0; i < 8; i++) {
        buffer[i] = (value >> (56 - 8 * i)) & 0xFF;
    }
}

//  ========== deserialize_bytes_to_uint64 ================================================
// deserialize bytes to uint64_t
static uint64_t deserialize_bytes_to_uint64(const uint8_t *buffer) {
    uint64_t value = 0;
    for (int i = 0; i < 8; i++) {
        value |= ((uint64_t)buffer[i] << (56 - 8 * i));
    }
    return value;
}

//  ========== app_eeprom_init =============================================================
int8_t app_eeprom_init(const struct device *dev)
{
	// check if the EEPROM device is ready
	if (!device_is_ready(dev)) {
		printk("%s: device is not ready\n", dev->name);
		return -1;
	}

	// erase the data storage partition
	int8_t ret = flash_erase(dev, SPI_FLASH_OFFSET, SPI_FLASH_SECTOR_SIZE*SPI_FLASH_SECTOR_NB);
	if (ret != 0){
		printf("MX25R64 flash erase failed. error: %d\n", ret);
		return -1;
	} else {
		printk("MX25R64 flash erase succeeded\n");
	}	
	return 1;
}

//  ========== app_eeprom_write ============================================================
// write data to EEPROM
int8_t app_eeprom_write(const struct device *dev, const uint8_t *data, size_t length)
{
    int8_t ret = flash_write(dev, SPI_FLASH_OFFSET, data, length);
    if (ret != 0) {
        printk("Eerror writing data. Error: %d\n", ret);
        return -1;
    }
    printk("successfully wrote %zu bytes to address 0x%X\n", length, SPI_FLASH_OFFSET);
    return 0;
}

//  ========== app_rom_read ================================================================
// Read data from EEPROM
int8_t app_eeprom_read(const struct device *dev, uint8_t *data, size_t length)
{
    int ret = flash_read(dev, SPI_FLASH_OFFSET, data, length);
    if (ret != 0) {
        printk("error reading data. Error: %d\n", ret);
        return -1;
    }
    printk("successfully read %zu bytes from address 0x%X\n", length, SPI_FLASH_OFFSET);
    return 0;
}

//  ======== app_rom_handler ===============================================================
int8_t app_eeprom_handler(const struct device *dev)
{
    uint8_t buffer[256] = {0}; // Enough space for timestamp and ADC data
    int16_t adc_data[MAX_RECORDS] = {0};
    uint64_t timestamp = 0;

    if (!device_is_ready(dev)) {
        printk("%s: device is not ready\n", dev->name);
        return -1;
    }

    // initialize RTC and get the current timestamp
    const struct device *rtc_dev = app_rtc_init();
    if (!rtc_dev) {
        printk("failed to initialize RTC device.\n");
        return -1;
    }
    timestamp = app_rtc_get_time(rtc_dev);

    // serialize timestamp into the buffer
    serialize_uint64_to_bytes(timestamp, buffer);

    // get ADC data and serialize it into the buffer
    for (int i = 0; i < MAX_RECORDS; i++) {
        adc_data[i] = app_nrf52_get_adc();
        buffer[8 + i * 2] = (adc_data[i] >> 8) & 0xFF; // high byte
        buffer[9 + i * 2] = adc_data[i] & 0xFF;        // low byte
    }

    // write buffer to EEPROM
    if (app_eeprom_write(dev, buffer, sizeof(buffer)) != 0) {
        return -1;
    }

    // read back and verify
    uint8_t read_buffer[256] = {0};
    if (app_eeprom_read(dev, read_buffer, sizeof(read_buffer)) != 0) {
        return -1;
    }

    // deserialize timestamp
    uint64_t read_timestamp = deserialize_bytes_to_uint64(read_buffer);
    printk("Read timestamp: %llu\n", read_timestamp);

    // deserialize and print ADC data
    for (int i = 0; i < MAX_RECORDS; i++) {
        int16_t read_adc = (read_buffer[8 + i * 2] << 8) | read_buffer[9 + i * 2];
        printk("Read ADC value [%d]: %d\n", i, read_adc);
    }
    return 0;
}


