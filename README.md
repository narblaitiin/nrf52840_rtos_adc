# Code for 6Sens Project : test of MDBT50Q SoC ADC (AIN0 - P0.02) and DS3231 RTC (I2C)

## Overview
This first code allows us to convert a voltage and digital values and adds processing to it:

 - take sample of sensor level (Analog-to-Digital), from filtering and ampliflying part of PCB (from geophone sensor to analog P0.02 of MDBT50Q)
 - get timestamp from DS3231 RTC (I2C device)
 - store the different values in an area of partitioned external QSPI flash memory MX25R64

This allows us to test the analog part of PCB and the internal ADC of the MDBT50Q. The final goal will be to send the samples to a lorawan server and clear the memory location once a day.

## Building and Running
The following commands clean build folder, build and flash the sample:

**Command to use**
````
west build -t pristine

west build -p always -b mdbt50q_lora_dev applications/nrf52840_rtos_adc

west flash --runner jlink