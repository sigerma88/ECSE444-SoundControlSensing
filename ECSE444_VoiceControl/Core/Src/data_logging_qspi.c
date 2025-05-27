/*
 * data_logging_qspi.c
 *
 *  Created on: Dec 1, 2024
 *      Author: siger
 */

#include "data_logging_qspi.h"

/* Private variables */
// QSPI tx and rx buffers
uint8_t qspi_aTxBuffer[BUFFER_SIZE];
uint8_t qspi_aRxBuffer[BUFFER_SIZE];
uint32_t current_write_addr[4] = {TEMP_ADDR, HUM_ADDR, ACC_ADDR, GYRO_ADDR};

// Sensor data ranges
float temp_max = 120.0f;   // Max Temp value
float temp_min = -40.0f;   // Min Temp Value
float hum_max = 100.0f;    // Max Hum Value
float hum_min = 0.0f;      // Min Hum value
float gyro_max = 1000.0f;  // Max Gyro value
float gyro_min = -1000.0f; // Min Gyro value
float acc_max = 3.0f;      // Max Accelerometer value
float acc_min = -3.0f;     // Min Accelerometer value

/* Private function prototypes */
uint8_t QuantizeFloatTo8Bit(float value, float minVal, float maxVal);
float Dequantize8BitToFloat(uint8_t value, float minVal, float maxVal);

/* Functions */

/**
 * @brief Quantize a float value to an 8-bit unsigned integer
 *
 * @param value    The value to quantize
 * @param minVal   The minimum value of the range
 * @param maxVal   The maximum value of the range
 * @return uint8_t The quantized value
 */
uint8_t QuantizeFloatTo8Bit(float value, float minVal, float maxVal)
{
  return (uint8_t)((value - minVal) / (maxVal - minVal) * 255.0f);
}

/**
 * @brief Dequantize an 8-bit unsigned integer to a float value
 *
 * @param value  The value to dequantize
 * @param minVal The minimum value of the range
 * @param maxVal The maximum value of the range
 * @return float The dequantized value
 */
float Dequantize8BitToFloat(uint8_t value, float minVal, float maxVal)
{
  return (value / 255.0f) * (maxVal - minVal) + minVal;
}

/**
 * @brief Initialize QSPI and test QSPI info correctness
 *
 */
void QspiInit(void)
{
  // Initialize QSPI
  if (BSP_QSPI_Init() != QSPI_OK)
  {
    printf("Failed to initialize QSPI\n\r");
    Error_Handler();
  }

  // Get QSPI info
  QSPI_Info qspiInfo;
  if (BSP_QSPI_GetInfo(&qspiInfo) != QSPI_OK)
  {
    printf("Failed to get QSPI info\n\r");
    Error_Handler();
  }

  // Test QSPI info correctness
  if (qspiInfo.FlashSize != MX25R6435F_FLASH_SIZE ||
      qspiInfo.EraseSectorSize != MX25R6435F_SECTOR_SIZE ||
      qspiInfo.EraseSectorsNumber != (MX25R6435F_FLASH_SIZE / MX25R6435F_SECTOR_SIZE) ||
      qspiInfo.ProgPageSize != MX25R6435F_PAGE_SIZE ||
      qspiInfo.ProgPagesNumber != (MX25R6435F_FLASH_SIZE / MX25R6435F_PAGE_SIZE))
  {
    printf("QSPI info incorrect\n\r");
    Error_Handler();
  }

  // Erase chip
  printf("Erasing chip\n\r");
  if (BSP_QSPI_Erase_Chip() != QSPI_OK)
  {
    printf("Failed to erase chip\n\r");
    Error_Handler();
  }
}

/**
 * @brief Store sensor data in QSPI memory
 *
 * @param sensor_choice The sensor choice (0: temperature, 1: humidity, 2: accelerometer, 3: gyroscope)
 * @param data The sensor data to store pointer
 */
void storeQspiData(uint8_t sensor_choice, float *data)
{
  uint32_t max_block_size = 0, start_address = 0;
  uint32_t data_size = 0;

  // Determine memory parameters based on sensor choice
  switch (sensor_choice)
  {
  case 0:
    start_address = TEMP_ADDR;
    max_block_size = TEMP_BLOCK_SIZE;
    data_size = sizeof(float);
    break;
  case 1:
    start_address = HUM_ADDR;
    max_block_size = HUM_BLOCK_SIZE;
    data_size = sizeof(float);
    break;
  case 2:
    start_address = ACC_ADDR;
    max_block_size = ACC_BLOCK_SIZE;
    data_size = 3 * sizeof(float);
    break;
  case 3:
    start_address = GYRO_ADDR;
    max_block_size = GYRO_BLOCK_SIZE;
    data_size = 3 * sizeof(float);
    break;
  default:
    printf("Invalid sensor choice\n\r");
    return;
  }

  uint32_t *write_addr = &current_write_addr[sensor_choice];

  // Convert float data to uint8_t array
  uint8_t data_bytes[data_size];
  memcpy(data_bytes, data, data_size);

  if (BSP_QSPI_Write(data_bytes, *write_addr, data_size) != QSPI_OK)
  {
    printf("Failed to write data at address: 0x%08lX\n\r", *write_addr);
    Error_Handler();
  }

  *write_addr += data_size;

  if (*write_addr >= (start_address + max_block_size))
  {
    printf("Memory full for sensor %d\n\r", sensor_choice);
    dumpQspiData();
    Error_Handler();
  }
}

/**
 * @brief Dump QSPI data to the console for all sensors
 *
 */
void dumpQspiData(void)
{
  printf("Dumping QSPI data\n\r");

  for (int i = 0; i < 4; i++)
  {
    float data[3] = {0};

    uint32_t start_address = 0;
    uint32_t end_address = current_write_addr[i];
    uint32_t data_size = 0;

    switch (i)
    {
    case 0:
      start_address = TEMP_ADDR;
      data_size = sizeof(float);
      break;
    case 1:
      start_address = HUM_ADDR;
      data_size = sizeof(float);
      break;
    case 2:
      start_address = ACC_ADDR;
      data_size = 3 * sizeof(float);
      break;
    case 3:
      start_address = GYRO_ADDR;
      data_size = 3 * sizeof(float);
      break;
    }

    printf("Reading data for sensor %d\n\r", i);

    uint32_t current_read = start_address;

    while (current_read < end_address)
    {
      // Create a buffer to read the data bytes
      uint8_t data_bytes[data_size];

      if (BSP_QSPI_Read(data_bytes, current_read, data_size) != QSPI_OK)
      {
        printf("Error reading data at address: 0x%08lX\n\r", current_read);
        Error_Handler();
      }

      // Convert uint8_t array back to float
      memcpy(data, data_bytes, data_size);

      switch (i)
      {
      case 0:
        printf("    Temperature = %f\n\r", data[0]);
        break;
      case 1:
        printf("    Humidity = %f\n\r", data[0]);
        break;
      case 2:
        printf("    Accelerometer x = %f, y = %f, z = %f\n\r", data[0], data[1], data[2]);
        break;
      case 3:
        printf("    Gyroscope x = %f, y = %f, z = %f\n\r", data[0], data[1], data[2]);
        break;
      }

      current_read += data_size;
    }

    printf("Finished reading data for sensor %d\n\r", i);
  }

  printf("Finished dumping QSPI data\n\r");
}