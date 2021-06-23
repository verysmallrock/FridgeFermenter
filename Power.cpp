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

void setHumidityControl(Direction direction, bool updateState) {
	Serial.println("Humidity " + String(direction) + " " + String(updateState));
	if (direction == Inactive) {
		digitalWrite(RELAY_PIN_HUMIDIFIER, POWER_OFF);
		digitalWrite(RELAY_PIN_DEHUMIDIFIER, POWER_OFF);
		if (updateState) {
			state.humidActive = false;
			state.dehumidActive = false;
		}
	} else if (direction == Increase) {
		state.humidityDirection = INCREASE;
		digitalWrite(RELAY_PIN_HUMIDIFIER, POWER_ON);
		digitalWrite(RELAY_PIN_DEHUMIDIFIER, POWER_OFF);
		if (updateState) {
			state.humidActive = true;
			state.dehumidActive = false;
		}
	} else if (direction == Decrease) {
		state.humidityDirection = DECREASE;
		digitalWrite(RELAY_PIN_HUMIDIFIER, POWER_OFF);
		digitalWrite(RELAY_PIN_DEHUMIDIFIER, POWER_ON);
		if (updateState) {
			state.humidActive = false;
			state.dehumidActive = true;
		}
	}
}

void setTempControl(Direction direction) {
	Serial.println("Temp " + String(direction));
	if (direction == Inactive) {
		state.heatingActive = false;
		state.coolingActive = false;
		digitalWrite(RELAY_PIN_HEAT, POWER_OFF);
		digitalWrite(RELAY_PIN_FRIDGE, POWER_OFF);
	} else if (direction == Increase) {
		state.tempDirection = INCREASE;
		state.heatingActive = true;
		state.coolingActive = false;
		digitalWrite(RELAY_PIN_HEAT, POWER_ON);
		digitalWrite(RELAY_PIN_FRIDGE, POWER_OFF);
	} else if (direction == Decrease) {
		state.tempDirection = DECREASE;
		state.heatingActive = false;
		state.coolingActive = true;
		digitalWrite(RELAY_PIN_HEAT, POWER_OFF);
		digitalWrite(RELAY_PIN_FRIDGE, POWER_ON);
	}
}
