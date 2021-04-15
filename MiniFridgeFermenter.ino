#include "Main.h"
#include "AppState.h"
#include "Display.h"
#include "Power.h"
#include "DHTesp.h"
#include "RotaryEncoder.h"
#include <IoAbstraction.h> // for TaskManager

void onEncoderClick(uint8_t, bool);
void onEncoderRotate(int);

void readAndRedraw() {
  if (state.currentAppMode == MODE_IDLE) {
    readSensors();
  }

  drawDisplay();
}

void logData() {
  if (state.currentAppMode != MODE_IDLE) {
    return;
  }
  logSensorsToCloud();
}

void updateRelayStates() {
  if (state.currentAppMode != MODE_IDLE) {
    return;
  }
  updateRelays();
}

void setup() {
  Serial.begin(115200);
  delay(50);

  mainSetup();
  Serial.println("Appstate load");
  loadAppState();
  setupInput(onEncoderClick, onEncoderRotate);
  delay(2000);
  updateDisplay(true, false, false);
  updateRelays();

  pinMode(FAN_PIN_1, OUTPUT);

  taskManager.scheduleFixedRate(100, readAndRedraw);
  //taskManager.scheduleFixedRate(15000, logData);
  taskManager.scheduleFixedRate(1000, updateRelayStates);
  taskManager.scheduleFixedRate(1000, exitEditingIfIdle);
}

void onEncoderRotate(int newValue) {
  onInputChange(newValue);
}

void onEncoderClick(uint8_t pin, bool heldDown) {
  if(heldDown) {
    onInputLongPress();
  } else {
    onInputPress();
  }
}

void loop() {
  taskManager.runLoop();
}
