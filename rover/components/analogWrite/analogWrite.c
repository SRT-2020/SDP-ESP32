#include <stdio.h>
#include "analogWrite.h"
#include "driver/ledc.h"
#include <math.h>

//The ESP32 supports 16 channels, 8 for High Speed 8 for Low Speed
#define LEDC_CHANNELS 16
//Let the driver select the clock automatically
#define LEDC_DEFAULT_CLK 0

uint8_t channels_resolution[LEDC_CHANNELS] = {0};

analog_write_channel_t _analog_write_channels[16] = {
    {-1, 5000, 13},
    {-1, 5000, 13},
    {-1, 5000, 13},
    {-1, 5000, 13},
    {-1, 5000, 13},
    {-1, 5000, 13},
    {-1, 5000, 13},
    {-1, 5000, 13},
    {-1, 5000, 13},
    {-1, 5000, 13},
    {-1, 5000, 13},
    {-1, 5000, 13},
    {-1, 5000, 13},
    {-1, 5000, 13},
    {-1, 5000, 13},
    {-1, 5000, 13}};

int min(int x, int y) {
	/*
	 * Get lowest number given 2 numbers
	 */
	return (x < y) ? x : y;
}

/*
 * This is taken from the ESP32 Arduino Source Code, this analogWrite code was
 * exempted and thus by extension, the dependency for this code is included.
 */
double ledcSetup(uint8_t chan, double freq, uint8_t bit_num)
{
    if(chan >= LEDC_CHANNELS){
        fprintf(stderr, "No more LEDC channels available! You can have maximum %u", LEDC_CHANNELS);
        return 0;
    }
    uint8_t group=(chan/8), timer=((chan/2)%4);

    ledc_timer_config_t ledc_timer = {
        .speed_mode       = group,
        .timer_num        = timer,
        .duty_resolution  = bit_num,
        .freq_hz          = freq,
        .clk_cfg          = LEDC_DEFAULT_CLK
    };
    ledc_timer_config(&ledc_timer);
    channels_resolution[chan] = bit_num;

    return ledc_get_freq(group,timer);
}

void ledcAttachPin(uint8_t pin, uint8_t chan)
{
     if(chan >= LEDC_CHANNELS){
        return;
    }
    uint8_t group=(chan/8), channel=(chan%8), timer=((chan/2)%4);

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = group,
        .channel        = channel,
        .timer_sel      = timer,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = pin,
        .duty           = 0,
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel);
}

void ledcWrite(uint8_t chan, uint32_t duty)
{
    if(chan >= LEDC_CHANNELS){
        return;
    }
    uint8_t group=(chan/8), channel=(chan%8);

    //Fixing if all bits in resolution is set = LEDC FULL ON
    uint32_t max_duty = (1 << channels_resolution[chan]) - 1;

    if(duty == max_duty){
        duty = max_duty + 1;
    }

    ledc_set_duty(group, channel, duty);
    ledc_update_duty(group, channel);
}

int analogWriteChannel(uint8_t pin)
{
  int channel = -1;

  // Check if pin already attached to a channel
  for (uint8_t i = 0; i < 16; i++)
  {
    if (_analog_write_channels[i].pin == pin)
    {
      channel = i;
      break;
    }
  }

  // If not, attach it to a free channel
  if (channel == -1)
  {
    for (uint8_t i = 0; i < 16; i++)
    {
      if (_analog_write_channels[i].pin == -1)
      {
        _analog_write_channels[i].pin = pin;
        channel = i;
        ledcSetup(channel, _analog_write_channels[i].frequency, _analog_write_channels[i].resolution);
        ledcAttachPin(pin, channel);
        break;
      }
    }
  }

  return channel;
}

void ALLanalogWriteFrequency(double frequency)
{
  for (uint8_t i = 0; i < 16; i++)
  {
    _analog_write_channels[i].frequency = frequency;
  }
}

void analogWriteFrequency(uint8_t pin, double frequency)
{
  int channel = analogWriteChannel(pin);

  // Make sure the pin was attached to a channel, if not do nothing
  if (channel != -1 && channel < 16)
  {
    _analog_write_channels[channel].frequency = frequency;
  }
}

void ALLanalogWriteResolution(uint8_t resolution)
{
  for (uint8_t i = 0; i < 16; i++)
  {
    _analog_write_channels[i].resolution = resolution;
  }
}

void analogWriteResolution(uint8_t pin, uint8_t resolution)
{
  int channel = analogWriteChannel(pin);

  // Make sure the pin was attached to a channel, if not do nothing
  if (channel != -1 && channel < 16)
  {
    _analog_write_channels[channel].resolution = resolution;
  }
}

void analogWrite(uint8_t pin, uint32_t value, uint32_t valueMax)
{
  /* Have to give valueMax a value, C does not allow for default parameters. */
  int channel = analogWriteChannel(pin);

  // Make sure the pin was attached to a channel, if not do nothing
  if (channel != -1 && channel < 16)
  {
    uint8_t resolution = _analog_write_channels[channel].resolution;
    uint32_t levels = pow(2, resolution);
    uint32_t duty = ((levels - 1) / valueMax) * min(value, valueMax);

    // write duty to LEDC
    ledcWrite(channel, duty);
  }
}