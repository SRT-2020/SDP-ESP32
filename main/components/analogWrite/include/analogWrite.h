#ifndef ESP32_ANALOG_WRITE
#define ESP32_ANALOG_WRITE

typedef struct analog_write_channel
{
  int pin;
  double frequency;
  int resolution;
} analog_write_channel_t;

int analogWriteChannel(int pin);

void ALLanalogWriteFrequency(double frequency);
void analogWriteFrequency(int pin, double frequency);

void ALLanalogWriteResolution(int resolution);
void analogWriteResolution(int pin, int resolution);

void analogWrite(int pin, int value, int valueMax);

#endif
