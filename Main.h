#ifndef MAIN_H
#define MAIN_H 1

#define DEBUG false

#define FAN_PIN_1 A0
#define FAN_PIN_2 A1

void mainSetup();
void reinitDisplayCheck();
void readSensors();
void validateMinMaxTemp();
void logSensorsToCloud();
void drawDisplay(bool drawNextPoint = false);
void updateRelays();
void updateFans();
void appModeChanged(bool configChanged);

void onInputPress();
void onInputLongPress();
void onInputChange(int direction);

#endif
