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

void changeCurrentConfig1Field(int delta) {
  int * fieldToEdit = getCurrentConfig1Field();
  *fieldToEdit = *fieldToEdit + delta;
  appStateChanged = true;
}

void exitEditingIfIdle() {
  if (state.currentAppMode != MODE_IDLE) {
    if ((millis() - state.lastInputTime) > state.maxIdleTime) {
      nextAppMode(MODE_IDLE);
    }
  }
}
