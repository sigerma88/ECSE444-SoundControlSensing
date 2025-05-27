/*
 * data_logging_qspi.h
 *
 *  Created on: Dec 1, 2024
 *      Author: siger
 */

#ifndef INC_DATA_LOGGING_QSPI_H_
#define INC_DATA_LOGGING_QSPI_H_

/* Includes */
#include "main.h"
#include "stm32l4s5i_iot01.h"
#include "stm32l4s5i_iot01_qspi.h"

/* Defines */
/*
 * Flash size available MX25R6435F_FLASH_SIZE = 0x800000 => 64 Mb => 8 MB
 * Total memory used: 983040 Bytes + 983040 Bytes + 2949120 Bytes + 2949120 Bytes = 7864320 Bytes = 0x780000
 * 
 * Temperature and humidity data are stored as 4-byte floats
 * Accelerometer and gyroscope data are stored as 3 floats each (x, y, z),
 * so each sample is 3 * 4 bytes = 12 bytes
 * Number of temperature samples: 983040 / 4 = 245760 samples
 * Number of humidity samples: 983040 / 4 = 245760 samples
 * Number of accelerometer samples: 2949120 / 12 = 245760 samples
 * Number of gyroscope samples: 2949120 / 12 = 245760 samples
 */
#define BUFFER_SIZE ((uint32_t)0x0100)
#define WRITE_READ_ADDR ((uint32_t)0x0050)               // 80 Bytes reserved
#define TEMP_ADDR WRITE_READ_ADDR                        // Starting address for temperature data
#define TEMP_BLOCK_SIZE 0x0F0000                         // 983040 Bytes for temperature data
#define HUM_ADDR ((uint32_t)TEMP_ADDR + TEMP_BLOCK_SIZE) // Next block for humidity
#define HUM_BLOCK_SIZE 0x0F0000                          // 983040 Bytes for humidity data
#define ACC_ADDR ((uint32_t)HUM_ADDR + HUM_BLOCK_SIZE)   // Next block for accelerometer
#define ACC_BLOCK_SIZE 0x2D0000                          // 2949120 Bytes for accelerometer data (x, y, z)
#define GYRO_ADDR ((uint32_t)ACC_ADDR + ACC_BLOCK_SIZE)  // Next block for gyroscope
#define GYRO_BLOCK_SIZE 0x2D0000                         // 2949120 Bytes for gyroscope data (x, y, z)

/* Public functions */
void QspiInit(void);
void storeQspiData(uint8_t sensor_choice, float* data);
void dumpQspiData(void);

#endif /* INC_DATA_LOGGING_QSPI_H_ */
