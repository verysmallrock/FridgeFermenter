#ifndef APPSTATE_INCLUDE
#define APPSTATE_INCLUDE

#include <Arduino.h>

#define INACTIVE 0
#define INCREASE 1
#define DECREASE 2

#define MODE_IDLE 0
#define MODE_CONFIG_1 1
#define MODE_MAX 1

enum Config1CurrentEditField {
	tempLow = 0, tempHigh = 1, tempFloat,
	humLow, humHigh, humFloat,
	fanDuration, fanPeriod,
	Exit,
	lastField
};

struct AppState {
	bool valid;

	/*  Editor State */
	unsigned long lastInputTime;
	unsigned long maxIdleTime;
	int currentAppMode;
	Config1CurrentEditField config1Field;
	boolean editingCurrentField;

	/*  Sensor State */
	float currentTemp;
	float currentHumidity;

	/*  Sensor config */
	int minTemp;
	int maxTemp;
	int targetMinTemp;
	int targetMaxTemp;
	int tempFloat;

	int minHumidity;
	int maxHumidity;
	int targetMinHumidity;
	int targetMaxHumidity;
	int humidityFloat;

	int fanDurationSeconds;
	int fanIntervalMinutes;

	/* Power activity */
	int tempDirection;
	bool heatingActive;
	bool coolingActive;

	int humidityDirection;
	bool humidActive;
	bool dehumidActive;
};
#define DEFAULT_STATE { \
	true, /* valid;*/ \
	0, /* unsigned long lastInputTime; */ \
	15000, /* unsigned long maxIdleTime; */ \
	MODE_IDLE, /* currentAppMode */ \
	tempLow, /* Config1CurrentEditField config1Field; */ \
	false, /* boolean editingCurrentField; */ \
	\
	0, /* float currentTemp; */ \
	0, /* float currentHumidity; */ \
	\
	40, /* int minTemp; */ \
	90, /* int maxTemp; */ \
	65, /* int targetMinTemp; */ \
	70, /* int targetMaxTemp; */ \
	3, /* int tempFloat; */ \
	\
	55, /* int minHumidity; */ \
	90, /* int maxHumidity; */ \
	70, /* int targetMinHumidity; */ \
	75, /* int targetMaxHumidity; */ \
	3, /* int humidityFloat; */ \
	\
	30, /* int fanDurationSeconds; */ \
	180, /* int fanIntervalMinutes; */ \
	\
	INACTIVE, /* int8_t tempDirection; */ \
	false, /* bool heatingActive; */ \
	false, /* bool coolingActive; */ \
	\
	INACTIVE, /* int8_t humidityDirection; */ \
	false, /* bool humidActive; */ \
	false, /* bool dehumidActive; */ \
}

extern AppState state;

void loadAppState();
void saveAppState();

void nextAppMode(int forceMode = NULL);
void nextConfig1EditField(int direction);
void changeCurrentConfig1Field(int delta);
void exitEditingIfIdle();

#endif
