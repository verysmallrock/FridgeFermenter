#ifndef POWER_INCLUDE
#define POWER_INCLUDE

#include "DHTesp.h"

#define RELAY_PIN_HUMIDIFIER 4
#define RELAY_PIN_DEHUMIDIFIER 5
#define RELAY_PIN_FRIDGE 6
#define RELAY_PIN_HEAT 7

#define POWER_ON LOW
#define POWER_OFF HIGH

typedef enum {
  Inactive  = 0,
  Increase  = 1,
  Decrease  = 2,
} Direction;


void setupPower();
void setHumidityControl(Direction direction, bool updateState = true);
void setTempControl(Direction direction);

#endif
