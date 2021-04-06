#include "Display.h"
#include "Main.h"
#include "DHTesp.h"
#include "RotaryEncoder.h"
#include<IoAbstraction.h> // for TaskManager

void onEncoderClick(uint8_t, bool);
void onEncoderRotate(int);

void readAndRedraw() {
  readSensors();
  drawDisplay();
}

void setup() {
  Serial.begin(115200);
  delay(50);
  mainSetup();
  setupInput(onEncoderClick, onEncoderRotate);
  delay(2000);
  updateDisplay(true, false, false);
  updateRelays();

  taskManager.scheduleFixedRate(100, readAndRedraw);
  taskManager.scheduleFixedRate(15000, logSensorsToCloud);
  taskManager.scheduleFixedRate(1000, updateRelays);
}

void onEncoderRotate(int newValue) {
  Serial.println("Encoder " + String(newValue));
}

void onEncoderClick(uint8_t pin, bool heldDown) {
  Serial.println("Click!");
}

void loop() {
  taskManager.runLoop();
}
