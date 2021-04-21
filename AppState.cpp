#include "AppState.h"
#include "Main.h"
#include <FlashStorage.h>

AppState state;
FlashStorage(state_store, AppState);

void loadAppState() {
  state = state_store.read();

  // NOTE: State is cleared when writing a new program to Arduino!
  if (state.valid == false) {
    Serial.println("State.valid is " + String(state.valid));
    state = DEFAULT_STATE;
  } else {
    Serial.println("State loaded");
  }
  state.lastFanUpdate = 0;
}

boolean appStateChanged = false;
void saveAppState() {
  if (!appStateChanged) {
    return;
  }
  Serial.println("Saving app state");
  state_store.write(state);
  appStateChanged = false;
}

void nextAppMode(int forceMode) {
  int lastAppMode = state.currentAppMode;
  if (forceMode != NULL) {
    state.currentAppMode = forceMode;
  } else {
    ++state.currentAppMode;
  }
  if (state.currentAppMode > MODE_MAX) {
    state.currentAppMode = MODE_IDLE;
  }

  if (lastAppMode != MODE_IDLE && state.currentAppMode == MODE_IDLE)
    saveAppState();
  appModeChanged();
}

int c1editFields[] = {tempLow, tempHigh, tempFloat, humLow, humHigh, humFloat, fanDuration, fanPeriod, Exit, lastField};
void nextConfig1EditField(int direction) {
  int field = int(state.config1Field);
  if (direction > 0) {
    ++field;
    if (field == int(lastField))
      field = tempLow;
  }
  else {
    --field;
    if (field < 0)
      field = Config1CurrentEditField(lastField - 1);
  }
  state.config1Field = Config1CurrentEditField(c1editFields[field]);
}

int * getCurrentConfig1Field() {
  switch(state.config1Field) {
    case tempLow: return &state.targetMinTemp;
    case tempHigh: return &state.targetMaxTemp;
    case tempFloat: return &state.tempFloat;
    case humLow: return &state.targetMinHumidity;
    case humHigh: return &state.targetMaxHumidity;
    case humFloat: return &state.humidityFloat;
    case fanDuration: return &state.fanDurationSeconds;
    case fanPeriod: return &state.fanIntervalMinutes;
  }
}


#define MIN_TEMP 36
#define MAX_TEMP 120
#define MIN_TEMP_RANGE 3
#define MIN_HUMIDITY 15
#define MAX_HUMIDITY 99
#define MIN_HUMIDITY_RANGE 3
#define MIN_FLOAT 1
#define MAX_FLOAT 30
#define MIN_FAN_DURATION 5 // seconds
#define MAX_FAN_DURATION 300
#define MIN_FAN_PERIOD 1 // minutes
#define MAX_FAN_PERIOD 999

int clamp(int value, int minValue, int maxValue) {
  return min(max(value, minValue), maxValue);
}

int validateCurrentConfig1Field(int currentField) {
  switch(state.config1Field) {
    case tempLow: return clamp(currentField, MIN_TEMP, MAX_TEMP - MIN_TEMP_RANGE);
    case tempHigh: return clamp(currentField, MIN_TEMP + MIN_TEMP_RANGE, MAX_TEMP);
    case tempFloat: return clamp(currentField, MIN_FLOAT, MAX_FLOAT);
    case humLow: return clamp(currentField, MIN_HUMIDITY, MAX_HUMIDITY - MIN_HUMIDITY_RANGE);
    case humHigh: return clamp(currentField, MIN_HUMIDITY + MIN_HUMIDITY_RANGE, MAX_HUMIDITY);
    case humFloat: return clamp(currentField, MIN_FLOAT, MAX_FLOAT);
    case fanDuration: return clamp(currentField, MIN_FAN_DURATION, MAX_FAN_DURATION);
    case fanPeriod: return clamp(currentField, MIN_FAN_PERIOD, MAX_FAN_PERIOD);
  }
}

void correctRelatedConfig1Field(int currentField) {
  // assume currentField is VALID
  switch(state.config1Field) {
    case tempLow: state.targetMaxTemp = clamp(state.targetMaxTemp, currentField + MIN_TEMP_RANGE, MAX_TEMP); break;
    case tempHigh: state.targetMinTemp = clamp(state.targetMinTemp, MIN_TEMP, currentField - MIN_TEMP_RANGE); break;
    case tempFloat: return;
    case humLow: state.targetMaxHumidity = clamp(state.targetMaxHumidity, currentField + MIN_HUMIDITY_RANGE, MAX_HUMIDITY); break;
    case humHigh: state.targetMinHumidity = clamp(state.targetMinHumidity, MIN_HUMIDITY, currentField - MIN_HUMIDITY_RANGE);
    case humFloat: return;
    case fanDuration: return;
    case fanPeriod: return;
  }
}

void changeCurrentConfig1Field(int delta) {
  int * fieldToEdit = getCurrentConfig1Field();
  *fieldToEdit = validateCurrentConfig1Field(*fieldToEdit + delta);
  correctRelatedConfig1Field(*fieldToEdit);
  appStateChanged = true;
}

void exitEditingIfIdle() {
  if (state.currentAppMode != MODE_IDLE) {
    if ((millis() - state.lastInputTime) > state.maxIdleTime) {
      //nextAppMode(MODE_IDLE);
    }
  }
}
