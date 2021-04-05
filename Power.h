#ifndef POWER_INCLUDE
#define POWER_INCLUDE

#include "DHTesp.h"

#define RELAY_PIN_HUMIDIFIER 4
#define RELAY_PIN_DEHUMIDIFIER 5
#define RELAY_PIN_FRIDGE 6
#define RELAY_PIN_HEAT 7

void setupPower();
void activateHumidifier(bool active = LOW);
void activateDehumidifier(bool active = LOW);
void activateFridge(bool active = LOW);
void activateHeat(bool active = LOW);

#endif
