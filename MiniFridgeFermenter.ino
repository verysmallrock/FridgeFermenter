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
  } else if (state.currentAppMode == MODE_CONFIG_1 && now - readTimer > ConfigDisplayUpdateRate) {
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
  if (state.currentAppMode != MODE_IDLE) {
    return;
  }
  updateRelays();
}

void setup() {
  Serial.begin(115200);
  readTimer = millis();
  delay(50);

  mainSetup();
  Serial.println("Appstate load");
  setupInput(onEncoderClick, onEncoderRotate);
  delay(2000);
  updateDisplay(true, false, false);
  updateRelays();

  pinMode(FAN_PIN_1, OUTPUT);

  taskManager.scheduleFixedRate(220, readAndRedraw);
  taskManager.scheduleFixedRate(60000, logData);
  taskManager.scheduleFixedRate(2500, updateRelayStates);
  taskManager.scheduleFixedRate(2500, exitEditingIfIdle);
  taskManager.scheduleFixedRate(5000, updateFans);


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
