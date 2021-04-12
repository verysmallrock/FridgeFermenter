#include "Power.h"

void setupPower() {
	pinMode(RELAY_PIN_HUMIDIFIER, OUTPUT);
	pinMode(RELAY_PIN_DEHUMIDIFIER, OUTPUT);
	pinMode(RELAY_PIN_FRIDGE, OUTPUT);
	pinMode(RELAY_PIN_HEAT, OUTPUT);

	digitalWrite(RELAY_PIN_HUMIDIFIER, POWER_OFF);
	digitalWrite(RELAY_PIN_DEHUMIDIFIER, POWER_OFF);
	digitalWrite(RELAY_PIN_FRIDGE, POWER_OFF);
	digitalWrite(RELAY_PIN_HEAT, POWER_OFF);
}

void activateHumidifier(int active) {
	digitalWrite(RELAY_PIN_HUMIDIFIER, active);
}

void activateDehumidifier(int active) {
	digitalWrite(RELAY_PIN_DEHUMIDIFIER, active);
}

void activateFridge(int active) {
	digitalWrite(RELAY_PIN_FRIDGE, active);
}

void activateHeat(int active) {
	digitalWrite(RELAY_PIN_HEAT, active);
}
