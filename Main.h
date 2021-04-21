#ifndef MAIN_H
#define MAIN_H 1

#define DEBUG false

#define TEMPHUMIDITY_PIN A7
#define FAN_PIN_1 A0

void mainSetup();
void readSensors();
void validateMinMaxTemp();
void logSensorsToCloud();
void drawDisplay(bool drawNextPoint = false);
void updateRelays();
void updateFans();
void appModeChanged();

void onInputPress();
void onInputLongPress();
void onInputChange(int direction);

#endif
