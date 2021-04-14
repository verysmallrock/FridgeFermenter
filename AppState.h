#ifndef APPSTATE_INCLUDE
#define APPSTATE_INCLUDE

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
}

extern AppState state;

void loadAppState();
void saveAppState();

#endif
