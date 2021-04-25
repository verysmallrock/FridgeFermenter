#ifndef APPSTATE_INCLUDE
#define APPSTATE_INCLUDE

#include <Arduino.h>

#define INACTIVE 0
#define INCREASE 1
#define DECREASE 2

#define MODE_IDLE 0
#define MODE_CONFIG_1 1
#define MODE_CONFIG_2 2
#define MODE_MAX 2

enum Config1CurrentEditField {
	C1TempLow = 0, C1TempHigh = 1, C1TempFloat,
	C1HumLow, C1HumHigh, C1DehumFloat, C1HumFloat,
	C1FanDuration, C1FanPeriod,
	C1Next, C1Exit,
	C1LastField
};

enum Config2CurrentEditField {
	C2HumWhenCooling = 0,
	C2HumPeriod, C2HumBreak,
	C2Exit, C2LastField
};

struct AppState {
	bool valid;

	/*  Editor State */
	unsigned long lastInputTime;
	unsigned long maxIdleTime;
	int currentAppMode;
	Config1CurrentEditField config1Field;
	Config2CurrentEditField config2Field;
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

	// Config 2
	int humidifyWhenCooling;
	// Humidifier really pumps the humidity, so use an on/off schedule to reduce the
	// amount it goes over our upper bound
	int humidityPeriod; // seconds
	int humidityBreak; // seconds
};
#define DEFAULT_STATE { \
	true, /* valid;*/ \
	0, /* unsigned long lastInputTime; */ \
	15000, /* unsigned long maxIdleTime; */ \
	MODE_IDLE, /* currentAppMode */ \
	C1TempLow, /* Config1CurrentEditField config1Field; */ \
	C2HumWhenCooling, /*Config2CurrentEditField config2Field; */ \
	false, /* boolean editingCurrentField; */ \
	\
	0, /* float currentTemp; */ \
	0, /* float currentHumidity; */ \
	\
	47, /* int minTemp;  Graphed min temp */ \
	63, /* int maxTemp;  Graphed max temp */ \
	53, /* int targetMinTemp; User-chosen */ \
	55, /* int targetMaxTemp; User-chosen */ \
	3, /* int tempFloat; User-chosen */ \
	\
	73, /* int minHumidity; Graphed min */ \
	88, /* int maxHumidity;  Graphed max */ \
	75, /* int targetMinHumidity; User-chosen */ \
	80, /* int targetMaxHumidity; User-chosen */ \
	5, /* int dehumidityFloat; */ \
	5, /* int humidityFloat; User-chosen */ \
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
	\
	1, /* int humidifyWhenCooling; */ \
	15, /*int humidityPeriod; */ \
	15,  /* int humidityBreak; */ \
}

extern AppState state;

void loadAppState();
void saveAppState();

void nextAppMode(int forceMode = -1);
void nextConfigEditField(int direction);
void changeCurrentConfigField(int delta);
void exitEditingIfIdle();

#endif
