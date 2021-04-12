#include "Main.h"
#include "Display.h"
#include "Power.h"
#include "DHTesp.h"
#include "RotaryEncoder.h"
#include<IoAbstraction.h> // for TaskManager

void onEncoderClick(uint8_t, bool);
void onEncoderRotate(int);

#define FAN_PIN_1 A0

struct AppState {
  unsigned long lastInputTime;
  unsigned long maxIdleTime;
};
AppState appState = { 0, 15000 };

void nextAppMode(int forceMode = NULL) {
  if (forceMode != NULL) {
    currentAppMode = forceMode;
  } else {
    ++currentAppMode;
  }
  if (currentAppMode > MODE_MAX) {
    currentAppMode = MODE_IDLE;
  }
  appModeChanged();
}

void exitEditingIfIdle() {
  if (currentAppMode != MODE_IDLE) {
    if ((millis() - appState.lastInputTime) > appState.maxIdleTime) {
      nextAppMode(MODE_IDLE);
    }
  }
}

void readAndRedraw() {
  if (currentAppMode == MODE_IDLE) {
    readSensors();
  }

  drawDisplay();
}

void logData() {
  if (currentAppMode != MODE_IDLE) {
    return;
  }
  logSensorsToCloud();
}

void updateRelayStates() {
  if (currentAppMode != MODE_IDLE) {
    return;
  }
  updateRelays();
}

void setup() {
  Serial.begin(115200);
  delay(50);
  mainSetup();
  setupInput(onEncoderClick, onEncoderRotate);
  delay(2000);
  updateDisplay(true, false, false);
  updateRelays();

  pinMode(FAN_PIN_1, OUTPUT);

  taskManager.scheduleFixedRate(100, readAndRedraw);
  taskManager.scheduleFixedRate(15000, logData);
  taskManager.scheduleFixedRate(1000, updateRelayStates);
  taskManager.scheduleFixedRate(1000, exitEditingIfIdle);
}

void onEncoderRotate(int newValue) {
  Serial.println("Encoder " + String(newValue));
  appState.lastInputTime = millis();
}

bool fanOn = false;
void onEncoderClick(uint8_t pin, bool heldDown) {
  if (heldDown) {
    fanOn = !fanOn;
    analogWrite(FAN_PIN_1, fanOn ? 255: 0);
    return;
  }

  nextAppMode();
  appState.lastInputTime = millis();
}

void loop() {
  taskManager.runLoop();
}
