#ifndef PHOTO_CELL_CALIBRATOR_H
#define PHOTO_CELL_CALIBRATOR_H
#include <stdbool.h>
#include "driver/adc.h"
//#define LeftLEDPin 12
//#define RightLEDPin 4

typedef struct photoVals {
  int Left_Photo;
  int Right_Photo;
} photoVals;

void setupPhotoresistors(uint8_t leftLED, uint8_t rightLED, adc1_channel_t leftPhoto_ADC1_Channel, adc1_channel_t rightPhoto_ADC1_Channel, adc_bits_width_t analogResolution, adc_atten_t analogAtten, uint8_t directionPin);
int readRawPhotoVal(adc1_channel_t photoCellChannel);
void fullCalibration(adc1_channel_t leftPhotoresistor, adc1_channel_t rightPhotoresistor, adc_bits_width_t analogResolution, uint32_t calibrationTime);
int trackLine(adc1_channel_t LeftPhotoresistorChannel, adc1_channel_t RightPhotoresistorChannel, bool reverseColor, photoVals *photoValues, bool direction);

#endif
