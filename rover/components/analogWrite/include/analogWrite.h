#ifndef ESP32_ANALOG_WRITE
#define ESP32_ANALOG_WRITE

typedef struct analog_write_channel
{
  int8_t pin;
  double frequency;
  uint8_t resolution;
} analog_write_channel_t;

int analogWriteChannel(uint8_t pin);

void ALLanalogWriteFrequency(double frequency);
void analogWriteFrequency(uint8_t pin, double frequency);

void ALLanalogWriteResolution(uint8_t resolution);
void analogWriteResolution(uint8_t pin, uint8_t resolution);

void analogWrite(uint8_t pin, uint32_t value, uint32_t valueMax);

#endif