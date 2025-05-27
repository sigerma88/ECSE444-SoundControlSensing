/*
 * sensor_sampling.c
 *
 *  Created on: Dec 1, 2024
 *      Author: siger
 */

#include "sensor_sampling.h"

/* Private variables */

// Sensor values
float temp_value = 0;             // Measured temperature value (Celsius)
float hum_value = 0;              // Measured humidity value (%)
int16_t acc_value[3] = {0};       // Measured accelerometer value x, y, z (mg)
float acc_value_scaled[3] = {0};  // Scaled accelerometer value x, y, z (g)
float gyro_value[3] = {0};        // Measured gyro value x, y, z (mdps)
float gyro_value_scaled[3] = {0}; // Scaled gyro value x, y, z (dps)

// Messages
char temp_mode_init_end_msg[] = "====> Temperature sensor HTS221 initialized\n\r";
char hum_mode_init_end_msg[] = "====> Humidity sensor HTS221 initialized\n\r";
char acc_mode_init_end_msg[] = "====> Accelerometer sensor LSM6DSL initialized\n\r";
char gyro_mode_init_end_msg[] = "====> Gyroscope sensor LSM6DSL initialized\n\r";

/* Functions */

/**
 * @brief Initialize the sensors
 *
 */
void initializeSensor(void)
{
  if (BSP_TSENSOR_Init() != TSENSOR_OK)
  {
    Error_Handler();
  }
  printf(temp_mode_init_end_msg);

  if (BSP_HSENSOR_Init() != HSENSOR_OK)
  {
    Error_Handler();
  }
  printf(hum_mode_init_end_msg);

  if (BSP_ACCELERO_Init() != ACCELERO_OK)
  {
    Error_Handler();
  }
  printf(acc_mode_init_end_msg);

  if (BSP_GYRO_Init() != GYRO_OK)
  {
    Error_Handler();
  }
  printf(gyro_mode_init_end_msg);
}

/**
 * @brief Read sensor data
 *
 * @param sensor_choice Sensor choice (0: temperature, 1: humidity, 2: accelerometer, 3: gyroscope, other: do nothing)
 * @return float* Data address
 */
float *readSensorData(uint8_t sensor_choice)
{
  // data address to be returned
  float* data_address = 0;

  switch (sensor_choice)
  {
  case 0:
    temp_value = BSP_TSENSOR_ReadTemp();
    data_address = &temp_value;
    break;
  case 1:
    hum_value = BSP_HSENSOR_ReadHumidity();
    data_address = &hum_value;
    break;
  case 2:
    BSP_ACCELERO_AccGetXYZ(acc_value);
    acc_value_scaled[0] = acc_value[0] / 1000.0f;
    acc_value_scaled[1] = acc_value[1] / 1000.0f;
    acc_value_scaled[2] = acc_value[2] / 1000.0f;
    data_address = acc_value_scaled;
    break;
  case 3:
    BSP_GYRO_GetXYZ(gyro_value);
    gyro_value_scaled[0] = gyro_value[0] / 1000.0f;
    gyro_value_scaled[1] = gyro_value[1] / 1000.0f;
    gyro_value_scaled[2] = gyro_value[2] / 1000.0f;
    data_address = gyro_value_scaled;
    break;
  default:
    printf("Invalid sensor choice\n\r");
    break;
  }

  return data_address;
}

/**
 * @brief Transmit sensor data
 *
 * @param sensor_choice Sensor choice (0: temperature, 1: humidity, 2: accelerometer, 3: gyroscope, other: do nothing)
 */
void transmitSensorData(uint8_t sensor_choice)
{
  switch (sensor_choice)
  {
  case 0:
    printf("Temperature = %f\n\r", temp_value);
    break;
  case 1:
    printf("Humidity = %f\n\r", hum_value);
    break;
  case 2:
    printf("Accelerometer x= %f y= %f z= %f\n\r", acc_value_scaled[0], acc_value_scaled[1], acc_value_scaled[2]);
    break;
  case 3:
    printf("Gyroscope x= %f y= %f z= %f\n\r", gyro_value_scaled[0], gyro_value_scaled[1], gyro_value_scaled[2]);
    break;
  default:
    printf("Waiting for a valid frequency to be played\n\r");
    break;
  }
}
