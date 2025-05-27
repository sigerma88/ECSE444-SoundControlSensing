/*
 * microphone_fft.h
 *
 *  Created on: Dec 1, 2024
 *      Author: siger
 */

#ifndef INC_MICROPHONE_FFT_H_
#define INC_MICROPHONE_FFT_H_

/* Includes */
#include "main.h"
#include "arm_math.h"
#include <math.h>

/* Defines */
#define AUDIO_BUFFER_SIZE 1024
#define FFT_SIZE 512     // FFT length (must be a power of 2)
#define SAMPLE_RATE 8000 // Sampling frequency in Hz. This is the sampling rate of the microphone

/* Public variables */
extern arm_rfft_fast_instance_f32 S;
extern int32_t audioRecBuffer[AUDIO_BUFFER_SIZE];

/* Public functions */
void dmaRecHalfBuffCpltConversion(void);
void dmaRecBuffCpltConversion(void);
uint8_t commandTranslator(void);

#endif /* INC_MICROPHONE_FFT_H_ */
