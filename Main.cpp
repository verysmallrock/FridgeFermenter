#include "Main.h"
#include "AppState.h"
#include "Display.h"
#include "Power.h"
#include "DHT.h"
#include "Wifi.h"
#include "SheetLogger.h"

DHT dht(TEMPHUMIDITY_PIN, DHT22);

void mainSetup() {
  int line = 1;
  setupDisplay();
  printTitle("Initializing...");

  setupPower();
  printText(line++, "Power setup OK");
  dht.begin();
  printText(line++, "Temp setup OK");

  bool WifiOk = connectToWifi();
  printText(line++, WifiOk ? "Connected to Wifi" : "Wifi ERROR");

  bool sheetOk = setupSheetLogger();
  printText(line++, sheetOk ? "Sheetlogger OK": "Sheetlogger ERROR");
}

char buffer1[50];
char buffer2[50];

// This code assumes readSensors is called frequently to update currentTemp/currentHumidity
// All other tasks use those values for functionality.
void readSensors() {
  state.currentTemp = dht.convertCtoF(dht.readTemperature());
  state.currentHumidity = dht.readHumidity();
  validateMinMaxTemp();
}

String getTempActivityString() {
  switch(state.tempDirection) {
    case INACTIVE: return "Idle";
    case INCREASE: return "Heat";
    case DECREASE: return "Cool";
  }
  return "";
}

String getHumidityActivityString() {
  switch(state.humidityDirection) {
    case INACTIVE: return "Idle";
    case INCREASE: return "Humidify";
    case DECREASE: return "Dehumidify";
  }
  return "";
}

void initializeMinMax(float currentTemp, float currentHumidity) {
  state.minTemp = int(currentTemp) - state.tempWidth;
  state.maxTemp = int(currentTemp) + state.tempWidth;
  state.minHumidity = int(currentHumidity) - state.humidityWidth;
  state.maxHumidity = int(currentHumidity) + state.humidityWidth;

  if (state.currentAppMode == MODE_IDLE) {
    sprintf(buffer1, "%d F", state.minTemp);
    sprintf(buffer2, "%d F", state.maxTemp);
    printGraphBg(2, buffer1, buffer2, getTempActivityString());
    sprintf(buffer1, "%d%%", state.minHumidity);
    sprintf(buffer2, "%d%%", state.maxHumidity);
    printGraphBg(5, buffer1, buffer2, getHumidityActivityString());
  }
}

bool minMaxInitialized = false;
void validateMinMaxTemp() {
  if (!minMaxInitialized) {
    initializeMinMax(state.currentTemp, state.currentHumidity);
    minMaxInitialized = true;
  }
  if (isnan(state.currentHumidity) || isnan(state.currentTemp)) {
    Serial.println("Failed to read from DHT sensor. Setting to 50/50.");
    state.currentTemp = state.currentHumidity = 50;
  }
}

void appModeChanged() {
  updateDisplay(true, false, false);

  minMaxInitialized = false;
  validateMinMaxTemp();
}

void drawDisplay() {
  if (state.currentAppMode == MODE_IDLE) {
    updateDisplay(false, false, true);

    printTitle("Aging Box 0.1");
    sprintf(buffer1, "%0.1f F", state.currentTemp);
    printText(1, "Temperature", buffer1);
    printNextGraphPoint(2, (state.currentTemp - state.minTemp) / (state.maxTemp - state.minTemp));

    sprintf(buffer1, "%0.1f %%", state.currentHumidity);
    printText(4, "Humidity", buffer1);
    printNextGraphPoint(5, (state.currentHumidity - state.minHumidity) / (state.maxHumidity - state.minHumidity));
  } else if (state.currentAppMode == MODE_CONFIG_1) {
    updateDisplay(false, false, true);

    printTitle("Configuration");

    sprintf(buffer1, "%d - %d F", state.targetMinTemp, state.targetMaxTemp);
    //printText(1, "Temp", buffer1);
    String left[] = { "Temp" };
    uint16_t leftColors[] = { GRAY_600 };
    String right[] = { "70", " - ", "80 F" };
    uint16_t rightColors[] = { ORANGE_200, GRAY_600, GRAY_600 };
    printTextFancy(1, left, leftColors, 1, right, rightColors, 3);

    sprintf(buffer1, "%d F", state.tempFloat);
    printText(2, "  - Float", buffer1);

    sprintf(buffer1, "%d - %d %%", state.targetMinHumidity, state.targetMaxHumidity);
    printText(3, "Humidity", buffer1);

    sprintf(buffer1, "%d %%", state.humidityFloat);
    printText(4, "  - Float", buffer1);

    sprintf(buffer1, "%ds / %dm ", state.fanDurationSeconds, state.fanIntervalMinutes);
    printText(5, "Fan", buffer1);

    printText(6, "", "Exit");
  }
}

void onInputPress() {
  Serial.println("click");
  nextAppMode();
  state.lastInputTime = millis();
}

bool fanOn = false;
void onInputLongPress() {
  Serial.println("held down");
  fanOn = !fanOn;
  analogWrite(FAN_PIN_1, fanOn ? 255: 0);
}

void onInputChange(int direction) {
  Serial.println("Encoder " + String(direction));
  state.lastInputTime = millis();
}

bool toggle = false;
void updateRelays() {
  //toggle = !toggle;
  //activateHumidifier(toggle ? POWER_ON : POWER_OFF);
  // activateDehumidifier(toggle ? POWER_ON : POWER_OFF);
  // activateFridge(toggle ? POWER_ON : POWER_OFF);
  // activateHeat(toggle ? POWER_ON : POWER_OFF);
}

void logSensorsToCloud() {
  logTempHumidityToSheet(state.currentTemp, state.currentHumidity);
}
