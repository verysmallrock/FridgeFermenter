#include "Power.h"
#include "AppState.h"

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

void activateHumidifier(int active, bool updateState) {
	if (updateState)
		state.humidActive = active == POWER_ON;
	if (active == POWER_ON)
		state.humidityDirection = INCREASE;
	digitalWrite(RELAY_PIN_HUMIDIFIER, active);
}

void activateDehumidifier(int active) {
	state.dehumidActive = active == POWER_ON;
	if (active == POWER_ON)
		state.humidityDirection = DECREASE;
	digitalWrite(RELAY_PIN_DEHUMIDIFIER, active);
}

void activateFridge(int active) {
	state.coolingActive = active == POWER_ON;
	if (active == POWER_ON)
		state.tempDirection = DECREASE;
	digitalWrite(RELAY_PIN_FRIDGE, active);
}

void activateHeat(int active) {
	state.heatingActive = active == POWER_ON;
	if (active == POWER_ON)
		state.tempDirection = INCREASE;
	digitalWrite(RELAY_PIN_HEAT, active);
}
