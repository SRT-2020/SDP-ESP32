#include <stdio.h>
#include "Photo_Cell_Calibrator.h"
#include "time_control.h"
#include "pin_control.h"
#include "constants.h"
#include "esp_log.h"
//#include "motor_control.h"
//#include "delay.h"
//#include "motor_control.h"

bool calibrated = false;


typedef struct calibrationData {
	int LeftMax;
	int LeftMin;
	int RightMax;
	int RightMin;
	int endMax;
	int endMin;
} calibrationData;



/*int LeftMax = 0;
int LeftMin = 0;
int RightMax = 0;
int RightMin = 0;*/ // We have a 10-bit analog resolution

calibrationData frontCalibration =
{
	.LeftMax = 0,
	.LeftMin = 0,
	.RightMax = 0,
	.RightMin = 0,
	.endMax = 0,
	.endMin = 0
};

calibrationData backCalibration =
{
	.LeftMax = 0,
	.LeftMin = 0,
	.RightMax = 0,
	.RightMin = 0,
	.endMin = 0,
	.endMax = 0
};

uint8_t RightPhotoResistor;
uint8_t LeftPhotoResistor;
uint8_t RightLEDPin;
uint8_t LeftLEDPin;
uint8_t movementDirectionPin;

int constrain(int val, int minVal, int maxVal) {
	if (val > maxVal) {
		return maxVal;
	}
	else if (val < minVal) {
		return minVal;
	}
	else {
		return val;
	}
}

long map(long val, long minVal, long maxVal, long newMin, long newMax) {
	/*
	 * What this calculation is:
	 * Calculate the number that 1 step on the original scale would mean on the new scale
	 * i.e. 0 - 100, 1 whole integer step on this scale would map to a step of  0.1 on a scale of 0 - 10
	 * Then we do val - minVal to calculate total number of steps that have been made from minVal
	 * Now we convert these steps into the new scale steps by doing (val - minVal) * step ratio
	 * This gives us the total amount of steps from the new minimum value
	 * Then we offset this amount by newMin since we are mapping to this new scale
	 */
	return (val - minVal) * (newMax - newMin) / (maxVal - minVal) + newMin;
}

void setupPhotoresistors(uint8_t leftLED, uint8_t rightLED, adc1_channel_t leftPhoto_ADC1_Channel, adc1_channel_t rightPhoto_ADC1_Channel, adc_bits_width_t analogResolution, adc_atten_t analogAtten, uint8_t directionPin)
{
  LeftLEDPin = leftLED;
  RightLEDPin = rightLED;
  LeftPhotoResistor = leftPhoto_ADC1_Channel;
  RightPhotoResistor = rightPhoto_ADC1_Channel;
  movementDirectionPin = directionPin;

  //pinMode(LeftPhotoResistor, INPUT);
  //pinMode(RightPhotoResistor, INPUT);

  analogADC1Setup(leftPhoto_ADC1_Channel, analogAtten, analogResolution);
  analogADC1Setup(rightPhoto_ADC1_Channel, analogAtten, analogResolution);

  pinMode(LeftLEDPin, OUTPUT);
  pinMode(RightLEDPin, OUTPUT);
  pinMode(movementDirectionPin, OUTPUT);

  // Turn on both LEDs to signify calibration
  digitalWrite(LeftLEDPin, 1);
  digitalWrite(RightLEDPin, 1);

  fullCalibration(LeftPhotoResistor, RightPhotoResistor, 9+analogResolution, 10000);

  // Calibration Done.
  digitalWrite(LeftLEDPin, 0);
  digitalWrite(RightLEDPin, 0);
}


void CalibratePhotoresistors(adc1_channel_t senseChannel, int *senseMax, int *senseMin) {
  /*
   * This function will perform one iteration of a photo cell calibration.
   *
   * sensePin: A pointer to a value that represents the input pin reading our photo cell.
   * senseMax: A pointer to a value that represents the maximum value read by our photo cell, this will get calibrated.
   * senseMin: A pointer to a value that represents the minimum value read by our photo cell, this will get calibrated.
   */
  int sensorVal = analogADC1Read(senseChannel, true, 10);

  if (sensorVal > *senseMax) {
    *senseMax = sensorVal;
  }

  if (sensorVal < *senseMin) {
    *senseMin = sensorVal;
  }
}

void fullCalibration(adc1_channel_t leftPhotoresistor, adc1_channel_t rightPhotoresistor, adc_bits_width_t analogResolution, uint32_t calibrationTime) {
  /*
   * Calibrate both photo cells then return true to indicate success.
   *
   * calibrationTime: Defaults to 5000 ms if it is <= 0, simply sets the total time (in ms) we calibrate for.
   */
  unsigned long startTime = 0;
  //RightMax = (1<<analogResolution);
  //LeftMax = (1<<analogResolution);
  frontCalibration.RightMin = (1<<analogResolution);
  frontCalibration.LeftMin = (1<<analogResolution);

  backCalibration.RightMin = (1<<analogResolution);
  backCalibration.LeftMin = (1<<analogResolution);

  if (calibrationTime <= 0) {
	  calibrationTime = 5000;
  }

  // Calibrate one side
  startTime = millis();
  while ((millis() - startTime) < calibrationTime) {
	digitalWrite(movementDirectionPin, 1);
    CalibratePhotoresistors(leftPhotoresistor, &(frontCalibration.LeftMax), &(frontCalibration.LeftMin));
    CalibratePhotoresistors(rightPhotoresistor, &(frontCalibration.RightMax), &(frontCalibration.RightMin));
  }

  digitalWrite(LeftLEDPin, 0);
  digitalWrite(RightLEDPin, 0);

  startTime = millis();
  while ((millis() - startTime) < 1000);

  digitalWrite(LeftLEDPin, 1);
  digitalWrite(RightLEDPin, 1);

  // Calibrate the other
  startTime = millis();
  while((millis() - startTime) < calibrationTime) {
		digitalWrite(movementDirectionPin, 0);
	    CalibratePhotoresistors(leftPhotoresistor, &(backCalibration.LeftMax), &(backCalibration.LeftMin));
	    CalibratePhotoresistors(rightPhotoresistor, &(backCalibration.RightMax), &(backCalibration.RightMin));
  }
  calibrated = true;
}

int readRawPhotoVal(adc1_channel_t photoCellChannel) {
  /*
   * Singular version of readRawPhotoCells, does the same thing except only for one photocell.
   */
  return analogADC1Read(photoCellChannel, true, 10);
}

void readRawPhotoCells(adc1_channel_t LeftPhotoresistorChannel, adc1_channel_t RightPhotoresistorChannel, photoVals *photoValues) {
  /*
   * Fair self-explanatory: Reads raw data from both of our photo cells and stores into our photoVals struct which we have a pointer to.
   */
  if (calibrated) {
    if (photoValues == NULL) {return;}
    (*photoValues).Left_Photo = readRawPhotoVal(LeftPhotoresistorChannel);
    (*photoValues).Right_Photo = readRawPhotoVal(RightPhotoresistorChannel);
  }
}

void processPhotoCells(photoVals *photoValues, bool direction) {
  /*
   * This function will take in a photoVals struct defined in Photo_Cell_Calibrator.h
   *
   * It will then squeeze our newly read photoresistor values into our calibrated bounds and then further squeeze/map them into an integer value between 0 and 2.
   *
   * The reason why this works is that we are working with high contrast colors, thus if we detect light this leads to a higher sensor value and since we are working with
   * high contrast, the difference between light and dark is significant. Thus our high sensor value will map into a non-zero integer which will thus act as our LOGIC 1 boolean.
   * And vice versa, and dark color will then lead to a low sensor value which should map to zero, thus it will act as our LOGIC 0 boolean.
   *
   * The reason why I choose 0 to 2 rather than 0 to 1, is because I wanted to reduce the mapping range for 0. Thus we are less susceptible to slightly dark colors triggering our low logic.
   * Why we might want this, is because our line might have dust or something on it and it might trigger the sensor to be mapped to 0.
   *
   * Otherwise, it will do nothing if there are no values to work with
   *
   *
   */
  if (photoValues == NULL) {return;}

  if (direction) {
	  (*photoValues).Left_Photo = constrain((*photoValues).Left_Photo, frontCalibration.LeftMin, frontCalibration.LeftMax);
	  (*photoValues).Right_Photo = constrain((*photoValues).Right_Photo, frontCalibration.RightMin, frontCalibration.RightMax);

	  (*photoValues).Left_Photo = map((*photoValues).Left_Photo, frontCalibration.LeftMin, frontCalibration.LeftMax, 0, 3); // Anything greater than 0 indicates a light surface, anything that is 0 indicates a dark surface
	  (*photoValues).Right_Photo = map((*photoValues).Right_Photo, frontCalibration.RightMin, frontCalibration.RightMax, 0, 3);
  }
  else {
	  (*photoValues).Left_Photo = constrain((*photoValues).Left_Photo, backCalibration.LeftMin, backCalibration.LeftMax);
	  (*photoValues).Right_Photo = constrain((*photoValues).Right_Photo, backCalibration.RightMin, backCalibration.RightMax);

	  (*photoValues).Left_Photo = map((*photoValues).Left_Photo, backCalibration.LeftMin, backCalibration.LeftMax, 0, 3); // Anything greater than 0 indicates a light surface, anything that is 0 indicates a dark surface
	  (*photoValues).Right_Photo = map((*photoValues).Right_Photo, backCalibration.RightMin, backCalibration.RightMax, 0, 3);
  }
}

int handlePhotoCells(photoVals *photoValues, bool reverseColor) {
  /*
   * This will determine which sensor triggered (Assumes 2 photo cells each representing either left or right)
   * Returns an integer:
   * 1 for left side triggered
   * 2 for right side triggered
   * 0 for nothing triggered (normal)
   * -1 for an unexpected case/error
   */
  if (photoValues == NULL) {return -1;}
  int Left_Photo = (*photoValues).Left_Photo;
  int Right_Photo = (*photoValues).Right_Photo;
  if (Left_Photo == 1 && Right_Photo == 1) {
	  return 3;
  }
  if (reverseColor) { // Light is the line, dark is everything else
    if ((Left_Photo && !Right_Photo) || (!Left_Photo && Right_Photo)) { // If we detect dark, it must exclusively be detected on one side (This is the XOR of Left_Photo and Right_Photo)
      if (!Left_Photo) { // Left Side is dark (like asphalt)
        return 1;
      }
      else { // Right Side is dark (like asphalt)
        return 2;
      }
    }
    else if (Left_Photo && Right_Photo) { // Since we reversed the colors, we know we are on the line if we detect white on both sides, thus turn off the LEDs
      return 0;
    }
  }
  else { //Dark line, light outside
    if ((Left_Photo && !Right_Photo) || (!Left_Photo && Right_Photo)) { // If we detect dark, it must exclusively be detected on one side
      if (Left_Photo) { // Left Side is light (Like if we are using dark colored tape on a bright surface)
        return 1;
      }
      else { //Right Side is light (Like if we are using dark colored tape on a bright surface)
        return 2;
      }
    }
    else if (!Left_Photo && !Right_Photo) { // If both sensors detect dark then turn off the LEDs
      return 0;
    }
  }
  return -1;
}

int trackLine(adc1_channel_t LeftPhotoresistorChannel, adc1_channel_t RightPhotoresistorChannel, bool reverseColor, photoVals *photoValues, bool direction) {
  /*
   * This function will facilitate the reading and processing of our photo cell values. This function expects to read from 2 photo cells.
   *
   * This function was written to work with Trash-E which is a vehicular robot, thus it accepts some other parameters to interact with other components of the robot.
   */
  // Select which side to read photo resistor data from.
  if (direction) {
	  digitalWrite(movementDirectionPin, 1);
  }
  else {
	  digitalWrite(movementDirectionPin, 0);
  }
  readRawPhotoCells(LeftPhotoresistorChannel, RightPhotoresistorChannel, photoValues);
  //ESP_LOGE(TAG, "Left: %d, Right: %d", photoValues->Left_Photo, photoValues->Right_Photo);
  processPhotoCells(photoValues, direction);
  //ESP_LOGE(TAG, "Photo Cell result: %d, %d\n", (*photoValues).Left_Photo, (*photoValues).Right_Photo);
  //printf("Photo Cell result: %d, %d\n", (*photoValues).Left_Photo, (*photoValues).Right_Photo);
  return handlePhotoCells(photoValues, reverseColor);
  //printf("In Track Line, %d %d", (*photoValues).Left_Photo, (*photoValues).Right_Photo);
}