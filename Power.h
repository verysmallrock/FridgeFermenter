#ifndef POWER_INCLUDE
#define POWER_INCLUDE

#include "DHTesp.h"

#define RELAY_PIN_HUMIDIFIER 4
#define RELAY_PIN_DEHUMIDIFIER 5
#define RELAY_PIN_FRIDGE 6
#define RELAY_PIN_HEAT 7

#define POWER_ON LOW
#define POWER_OFF HIGH

void setupPower();
void activateHumidifier(int active, bool updateState = true);
void activateDehumidifier(int active);
void activateFridge(int active);
void activateHeat(int active);

#endif
