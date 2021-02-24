#ifndef POWER_INCLUDE
#define POWER_INCLUDE

#include "DHTesp.h"

#define RELAY_PIN_HUMIDIFIER D4
#define RELAY_PIN_DEHUMIDIFIER D3
#define RELAY_PIN_FRIDGE D2
#define RELAY_PIN_HEAT D1

void setupPower();
void activateHumidifier(bool active = LOW);
void activateDehumidifier(bool active = LOW);
void activateFridge(bool active = LOW);
void activateHeat(bool active = LOW);

#endif
