#include "Power.h"

void setupPower() {
	pinMode(RELAY_PIN_HUMIDIFIER, OUTPUT);
	pinMode(RELAY_PIN_DEHUMIDIFIER, OUTPUT);
	pinMode(RELAY_PIN_FRIDGE, OUTPUT);
	pinMode(RELAY_PIN_HEAT, OUTPUT);

	digitalWrite(RELAY_PIN_HUMIDIFIER, LOW);
	digitalWrite(RELAY_PIN_DEHUMIDIFIER, LOW);
	digitalWrite(RELAY_PIN_FRIDGE, LOW);
	digitalWrite(RELAY_PIN_HEAT, LOW);
}

void activateHumidifier(bool active) {
	digitalWrite(RELAY_PIN_HUMIDIFIER, active);
}

void activateDehumidifier(bool active) {
	digitalWrite(RELAY_PIN_DEHUMIDIFIER, active);
}

void activateFridge(bool active) {
	digitalWrite(RELAY_PIN_FRIDGE, active);
}

void activateHeat(bool active) {
	digitalWrite(RELAY_PIN_HEAT, active);
}
