/*
 * Copyright (c) 2025
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_EEPROM_H
#define APP_EEPROM_H

//  ========== includes ====================================================================
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/flash.h>

//  ========== defines =====================================================================
#define SPI_FLASH_DEVICE        DT_COMPAT_GET_ANY_STATUS_OKAY(nordic_qspi_nor)
#define SPI_FLASH_OFFSET		0x00000
#define SPI_FLASH_SECTOR_SIZE	4096   // in bytes
#define SPI_FLASH_SECTOR_NB     1
#define MAX_RECORDS             128    // max number of ADC records                     

//  ========== prototypes ==================================================================
int8_t app_eeprom_init(const struct device *dev);
int8_t app_eeprom_write(const struct device *dev, const uint8_t *data, size_t length);
int8_t app_eeprom_read(const struct device *dev, uint8_t *data, size_t length);
int8_t app_eeprom_handler(const struct device *dev);
static void serialize_uint64_to_bytes(uint64_t value, uint8_t *buffer);
static uint64_t deserialize_bytes_to_uint64(const uint8_t *buffer);

#endif /* APP_EEPROM_H */