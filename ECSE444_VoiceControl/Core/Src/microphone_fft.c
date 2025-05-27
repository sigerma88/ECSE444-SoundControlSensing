/*
 * microphone_fft.c
 *
 *  Created on: Dec 1, 2024
 *      Author: siger
 */

#include "microphone_fft.h"

/* Private variables */

// Audio buffer
int32_t audioRecBuffer[AUDIO_BUFFER_SIZE];
float32_t audioPlayBuffer[AUDIO_BUFFER_SIZE];

// FFT
float32_t inputSignal[FFT_SIZE];      // Input signal buffer
float32_t fftOutput[FFT_SIZE];        // FFT output buffer (complex interleaved)
float32_t fftMagnitude[FFT_SIZE / 2]; // Magnitude buffer (half-spectrum)
float32_t dominantFrequency;
arm_rfft_fast_instance_f32 S;

/* Private function prototypes */
void generateSineWave(float32_t *buffer, uint32_t size, float frequency, float amplitude, float sampleRate);
float32_t findDominantFrequency(float32_t *magnitudes, uint32_t size, float sampleRate);
void computeMagnetudeFFT(float32_t *FFTresult, uint32_t size, float32_t *MagnOutput);
int detectActionChoice(float32_t dominantFreq);

/* Functions */

/**
 * @brief Generate a sine wave
 *
 * @param buffer pointer to the buffer to store the sine wave
 * @param size size of the buffer
 * @param frequency frequency of the sine wave
 * @param amplitude amplitude of the sine wave
 * @param sampleRate sampling rate of the sine wave
 */
void generateSineWave(float32_t *buffer, uint32_t size, float frequency, float amplitude, float sampleRate)
{
  for (uint32_t i = 0; i < size; i++)
  {
    buffer[i] = amplitude * sinf(2 * PI * frequency * i / sampleRate);
  }
}

/**
 * @brief Compute the magnitude of the FFT
 *
 * @param FFTresult pointer to the FFT result buffer
 * @param size size of the FFT result buffer
 * @param MagnOutput pointer to the magnitude output buffer
 */
void computeMagnetudeFFT(float32_t *FFTresult, uint32_t size, float32_t *MagnOutput)
{
  float32_t real;
  float32_t imag;
  for (uint32_t i = 0; i < size / 2; i++)
  {
    real = FFTresult[2 * i];
    imag = FFTresult[2 * i + 1];
    MagnOutput[i] = sqrtf(real * real + imag * imag);
  }
}

/**
 * @brief Find the dominant frequency
 *
 * @param magnitudes pointer to the magnitudes buffer
 * @param size size of the magnitudes buffer
 * @param sampleRate sampling rate of the signal
 * @return float32_t dominant frequency
 */
float32_t findDominantFrequency(float32_t *magnitudes, uint32_t size, float sampleRate)
{
  uint32_t maxIndex = 0;
  float32_t maxMagnitude = 0;

  // Find index of max magn and ignore DC i.e. the first few indexes
  for (uint32_t i = 2; i < size; i++)
  {
    if (magnitudes[i] > maxMagnitude)
    {
      maxMagnitude = magnitudes[i];
      maxIndex = i;
    }
  }

  // find freq corresponding to max index
  return (maxIndex * sampleRate) / (2 * size);
}

/**
 * @brief Detect the action choice
 *
 * @param dominantFreq dominant frequency
 * @return int action choice
 */
int detectActionChoice(float32_t dominantFreq)
{
  const float32_t frequencies[] = {440.0f, 587.33f, 622.25f, 739.99f, 830.61f};
  const int numFrequencies = sizeof(frequencies) / sizeof(frequencies[0]);

  // Variable to store the closest index
  int closestIndex = 0;
  float32_t minDifference = fabs(dominantFreq - frequencies[0]);

  // Iterate over the reference frequencies to find the closest one
  for (int i = 1; i < numFrequencies; i++)
  {
    float32_t difference = fabs(dominantFreq - frequencies[i]);
    if (difference < minDifference)
    {
      minDifference = difference;
      closestIndex = i;
    }
  }

  if (minDifference >= 16)
  {           // the freq needs to be within 16 Hz (2 freq bins) to make a confident pick
    return 5; // 5 means non conclusive measurement
  }

  return closestIndex; // Return index of the closest frequency
}

/**
 * @brief Convert the half buffer complete
 *
 */
void dmaRecHalfBuffCpltConversion(void)
{
  for (int i = 0; i < AUDIO_BUFFER_SIZE / 2; i++)
  {
    audioPlayBuffer[i] = (float32_t)(audioRecBuffer[i] >> 8);
  }
}

/**
 * @brief Convert the buffer complete
 *
 */
void dmaRecBuffCpltConversion(void)
{
  for (int i = AUDIO_BUFFER_SIZE / 2; i < AUDIO_BUFFER_SIZE; i++)
  {
    audioPlayBuffer[i] = (float32_t)(audioRecBuffer[i] >> 8);
  }
}

/**
 * @brief Command translator
 *
 * @return uint8_t sensor choice
 */
uint8_t commandTranslator(void)
{
  arm_rfft_fast_f32(&S, audioPlayBuffer, fftOutput, 0);
  computeMagnetudeFFT(fftOutput, FFT_SIZE, fftMagnitude);
  dominantFrequency = findDominantFrequency(fftMagnitude, FFT_SIZE / 2, SAMPLE_RATE);
  printf("Dominant Frequency: %.2f Hz\r\n", dominantFrequency);
  uint8_t sensor_choice = detectActionChoice(dominantFrequency);
  printf("sensor choice: %i \r\n", sensor_choice);
  return sensor_choice;
}
