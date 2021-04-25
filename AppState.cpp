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

int c1editFields[] = {C1TempLow, C1TempHigh, C1TempFloat, C1HumLow, C1HumHigh, C1DehumFloat, C1HumFloat, C1FanDuration, C1FanPeriod, C1Next, C1Exit, C1LastField};
void nextConfig1EditField(int direction) {
  int field = int(state.config1Field);
  if (direction > 0) {
    ++field;
    if (field == int(C1LastField))
      field = C1TempLow;
  }
  else {
    --field;
    if (field < 0)
      field = Config1CurrentEditField(C1LastField - 1);
  }
  state.config1Field = Config1CurrentEditField(c1editFields[field]);
}

int * getCurrentConfig1Field() {
  switch(state.config1Field) {
    case C1TempLow: return &state.targetMinTemp;
    case C1TempHigh: return &state.targetMaxTemp;
    case C1TempFloat: return &state.tempFloat;
    case C1HumLow: return &state.targetMinHumidity;
    case C1HumHigh: return &state.targetMaxHumidity;
    case C1DehumFloat: return &state.dehumidityFloat;
    case C1HumFloat: return &state.humidityFloat;
    case C1FanDuration: return &state.fanDurationSeconds;
    case C1FanPeriod: return &state.fanIntervalMinutes;
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
    case C1TempLow: return clamp(currentField, MIN_TEMP, MAX_TEMP - MIN_TEMP_RANGE);
    case C1TempHigh: return clamp(currentField, MIN_TEMP + MIN_TEMP_RANGE, MAX_TEMP);
    case C1TempFloat: return clamp(currentField, MIN_FLOAT, MAX_FLOAT);
    case C1HumLow: return clamp(currentField, MIN_HUMIDITY, MAX_HUMIDITY - MIN_HUMIDITY_RANGE);
    case C1HumHigh: return clamp(currentField, MIN_HUMIDITY + MIN_HUMIDITY_RANGE, MAX_HUMIDITY);
    case C1DehumFloat: return clamp(currentField, MIN_FLOAT, MAX_FLOAT);
    case C1HumFloat: return clamp(currentField, MIN_FLOAT, MAX_FLOAT);
    case C1FanDuration: return clamp(currentField, MIN_FAN_DURATION, MAX_FAN_DURATION);
    case C1FanPeriod: return clamp(currentField, MIN_FAN_PERIOD, MAX_FAN_PERIOD);
  }
}

void correctRelatedConfig1Field(int currentField) {
  // assume currentField is VALID
  switch(state.config1Field) {
    case C1TempLow: state.targetMaxTemp = clamp(state.targetMaxTemp, currentField + MIN_TEMP_RANGE, MAX_TEMP); break;
    case C1TempHigh: state.targetMinTemp = clamp(state.targetMinTemp, MIN_TEMP, currentField - MIN_TEMP_RANGE); break;
    case C1TempFloat: return;
    case C1HumLow: state.targetMaxHumidity = clamp(state.targetMaxHumidity, currentField + MIN_HUMIDITY_RANGE, MAX_HUMIDITY); break;
    case C1HumHigh: state.targetMinHumidity = clamp(state.targetMinHumidity, MIN_HUMIDITY, currentField - MIN_HUMIDITY_RANGE);
    case C1DehumFloat: return;
    case C1HumFloat: return;
    case C1FanDuration: return;
    case C1FanPeriod: return;
  }
}

void changeCurrentConfig1Field(int delta) {
  if (state.config1Field == C1Next || state.config1Field == C1Exit)
    return;
  int * fieldToEdit = getCurrentConfig1Field();
  *fieldToEdit = validateCurrentConfig1Field(*fieldToEdit + delta);
  correctRelatedConfig1Field(*fieldToEdit);
  appStateChanged = true;
}

void exitEditingIfIdle() {
  if (state.currentAppMode != MODE_IDLE) {
    if ((millis() - state.lastInputTime) > state.maxIdleTime) {
      nextAppMode(MODE_IDLE);
    }
  }
}
