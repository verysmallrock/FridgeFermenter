#ifndef APPSTATE_INCLUDE
#define APPSTATE_INCLUDE

#define INACTIVE 0
#define INCREASE 1
#define DECREASE 2

struct AppState {
	bool valid;

	/*  Editor State */
	unsigned long lastInputTime;
	unsigned long maxIdleTime;

	/*  Sensor State */
	float currentTemp;
	float currentHumidity;

	/*  Sensor config */
	int minTemp;
	int maxTemp;
	int targetMinTemp;
	int targetMaxTemp;
	int tempFloat;
	int tempWidth;

	int minHumidity;
	int maxHumidity;
	int targetMinHumidity;
	int targetMaxHumidity;
	int humidityFloat;
	int humidityWidth;

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
	\
	0, /* float currentTemp; */ \
	0, /* float currentHumidity; */ \
	\
	40, /* int minTemp; */ \
	90, /* int maxTemp; */ \
	65, /* int targetMinTemp; */ \
	70, /* int targetMaxTemp; */ \
	3, /* int tempFloat; */ \
	10, /* int tempWidth; */ \
	\
	55, /* int minHumidity; */ \
	90, /* int maxHumidity; */ \
	70, /* int targetMinHumidity; */ \
	75, /* int targetMaxHumidity; */ \
	3, /* int humidityFloat; */ \
	10, /* int humidityWidth; */ \
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

#endif
