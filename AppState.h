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
	C1TempLow = 0, C1TempHigh = 1, C1TempFloat,
	C1HumLow, C1HumHigh, C1DehumFloat, C1HumFloat,
	C1FanDuration, C1FanPeriod,
	C1Next, C1Exit,
	C1LastField
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
	int dehumidityFloat;
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

	bool fanActive;
	unsigned long lastFanUpdate;
};
#define DEFAULT_STATE { \
	true, /* valid;*/ \
	0, /* unsigned long lastInputTime; */ \
	15000, /* unsigned long maxIdleTime; */ \
	MODE_IDLE, /* currentAppMode */ \
	C1TempLow, /* Config1CurrentEditField config1Field; */ \
	false, /* boolean editingCurrentField; */ \
	\
	0, /* float currentTemp; */ \
	0, /* float currentHumidity; */ \
	\
	47, /* int minTemp;  Graphed min temp */ \
	63, /* int maxTemp;  Graphed max temp */ \
	53, /* int targetMinTemp; User-chosen */ \
	57, /* int targetMaxTemp; User-chosen */ \
	4, /* int tempFloat; User-chosen */ \
	\
	73, /* int minHumidity; Graphed min */ \
	88, /* int maxHumidity;  Graphed max */ \
	79, /* int targetMinHumidity; User-chosen */ \
	82, /* int targetMaxHumidity; User-chosen */ \
	2, /* int dehumidityFloat; */ \
	7, /* int humidityFloat; User-chosen */ \
	\
	30, /* int fanDurationSeconds; User-chosen */ \
	180, /* int fanIntervalMinutes; User-chosen */ \
	\
	INACTIVE, /* int8_t tempDirection; */ \
	false, /* bool heatingActive; */ \
	false, /* bool coolingActive; */ \
	\
	INACTIVE, /* int8_t humidityDirection; */ \
	false, /* bool humidActive; */ \
	false, /* bool dehumidActive; */ \
	false, /* bool fanActive; */ \
	0, /*unsigned long lastFanUpdate; */ \
}

extern AppState state;

void loadAppState();
void saveAppState();

void nextAppMode(int forceMode = NULL);
void nextConfig1EditField(int direction);
void changeCurrentConfig1Field(int delta);
void exitEditingIfIdle();

#endif
