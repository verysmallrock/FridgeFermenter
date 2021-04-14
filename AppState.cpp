#include "AppState.h"
#include <FlashStorage.h>

AppState state;
FlashStorage(state_store, AppState);

void loadAppState() {
  state = state_store.read();

  // NOTE: State is cleared when writing a new program to Arduino!
  if (state.valid == false) {
    Serial.println("State.valid is " + String(state.valid));
    state = DEFAULT_STATE;
    Serial.println("initialzed and saving state" + String(state.valid) + " " + String(state.lastInputTime));
    saveAppState();
  } else {
    Serial.println("State loaded");
  }
}

void saveAppState() {
  // uncomment this later when we're ready to roll
  //state_store.write(state);
}
