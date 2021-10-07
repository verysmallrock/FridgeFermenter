#include "Main.h"
#include "AppState.h"
#include "Display.h"
#include "Power.h"
#include "DHTesp.h"
#include "RotaryEncoder.h"
#include <IoAbstraction.h> // for TaskManager

void onEncoderClick(uint8_t, bool);
void onEncoderRotate(int);


int IdleDisplayUpdateRate = 10000;
int ConfigDisplayUpdateRate = 220; // update fast in config mode
unsigned long readTimer;

void readAndRedraw() {
  unsigned long now = millis();
  if (state.currentAppMode == MODE_IDLE) {
    readSensors();
    bool drawNextPoint = now - readTimer > IdleDisplayUpdateRate;
    drawDisplay(drawNextPoint);
    if (drawNextPoint)
      readTimer = now;
  } else if ((state.currentAppMode == MODE_CONFIG_1 || state.currentAppMode == MODE_CONFIG_2) && now - readTimer > ConfigDisplayUpdateRate) {
    drawDisplay();
    readTimer = now;
  }
}

void logData() {
  if (state.currentAppMode != MODE_IDLE) {
    return;
  }
  logSensorsToCloud();
}

void updateRelayStates() {
  if (state.currentAppMode != MODE_IDLE || state.relayControl == 0) {
    return;
  }
  updateRelays();
}

void setup() {
  Serial.begin(9600);
  readTimer = millis();
  Serial.println("Booting up...");
  delay(2000);

  mainSetup();
  setupInput(onEncoderClick, onEncoderRotate);
  updateDisplay(true, false, false);
  updateRelays();

  pinMode(FAN_PIN_1, OUTPUT);
  pinMode(FAN_PIN_2, OUTPUT);

  taskManager.scheduleFixedRate(220, readAndRedraw);
  taskManager.scheduleFixedRate(60000, logData);
  taskManager.scheduleFixedRate(2400, updateRelayStates);
  taskManager.scheduleFixedRate(2500, exitEditingIfIdle);
  taskManager.scheduleFixedRate(5000, updateFans);
  // Can't schedule over 1 hour, so check often but only reset sometimes.
  // https://github.com/davetcc/TaskManagerIO/issues/8
  taskManager.scheduleFixedRate(60, reinitDisplayCheck, TIME_SECONDS); // having LCD reliability issues....
  taskManager.scheduleFixedRate(60, rebootCheck, TIME_SECONDS);

  // init display
  readSensors();
  drawDisplay(true);
}

void onEncoderRotate(int newValue) {
  onInputChange(newValue);
}

void onEncoderClick(uint8_t pin, bool heldDown) {
  readTimer = millis();
  if(heldDown) {
    onInputLongPress();
  } else {
    onInputPress();
  }
}

void loop() {
  taskManager.runLoop();
}
