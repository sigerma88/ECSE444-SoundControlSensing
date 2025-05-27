/*
 * sensor_sampling.h
 *
 *  Created on: Dec 1, 2024
 *      Author: siger
 */

#ifndef INC_SENSOR_SAMPLING_H_
#define INC_SENSOR_SAMPLING_H_

/* Includes */
#include "main.h"
#include "stm32l4s5i_iot01.h"
#include "stm32l4s5i_iot01_tsensor.h"
#include "stm32l4s5i_iot01_hsensor.h"
#include "stm32l4s5i_iot01_accelero.h"
#include "stm32l4s5i_iot01_gyro.h"

/* Public functions */
void initializeSensor(void);
void transmitSensorData(uint8_t sensor_choice);
float* readSensorData(uint8_t sensor_choice);

#endif /* INC_SENSOR_SAMPLING_H_ */
