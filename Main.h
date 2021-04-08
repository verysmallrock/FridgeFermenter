#ifndef MAIN_H
#define MAIN_H 1

#define MODE_IDLE 0
#define MODE_CONFIG_1 1
#define MODE_MAX 1
extern int currentAppMode;

void mainSetup();
void readSensors();
void validateMinMaxTemp();
void logSensorsToCloud();
void drawDisplay();
void updateRelays();
void appModeChanged();

#endif
