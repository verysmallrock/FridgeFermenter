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
  int floatRange = int(state.tempFloat * 1.8);
  state.minTemp = state.targetMinTemp - floatRange;
  state.maxTemp = state.targetMaxTemp + floatRange;

  int humidityFloat = int(state.humidityFloat * 1.8);
  state.minHumidity = state.targetMinHumidity - floatRange;
  state.maxHumidity = state.targetMaxHumidity + floatRange;

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

uint16_t _config1FieldColor(Config1CurrentEditField field) {
  if (int(state.config1Field) == int(field)) {
    if (state.editingCurrentField) {
      int blinkPeriod = 900;
      if ((millis() % blinkPeriod) < blinkPeriod / 3)
        return GRAY_100;
      else
        return GREEN_200;
    }
    else
      return YELLOW_200;
  } else {
    return GRAY_600;
  }
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

    String left[5];
    uint16_t leftColors[5];
    String right[5];
    uint16_t rightColors[5];

    left[0] = "Temp";
    leftColors[0] = GRAY_600;
    sprintf(buffer1, "%d", state.targetMinTemp); right[0] = buffer1;
    sprintf(buffer1, " - "); right[1] = buffer1;
    sprintf(buffer1, "%d F", state.targetMaxTemp); right[2] = buffer1;
    rightColors[0] = _config1FieldColor(tempLow);
    rightColors[1] = GRAY_600;
    rightColors[2] = _config1FieldColor(tempHigh);
    printTextFancy(1, left, leftColors, 1, right, rightColors, 3);

    left[0] = "  - Float";
    leftColors[0] = GRAY_600;
    sprintf(buffer1, "     %d F", state.tempFloat); right[0] = buffer1;
    rightColors[0] = _config1FieldColor(tempFloat);
    printTextFancy(2, left, leftColors, 1, right, rightColors, 1);

    left[0] = "Humidity";
    leftColors[0] = GRAY_600;
    sprintf(buffer1, "%d", state.targetMinHumidity); right[0] = buffer1;
    sprintf(buffer1, " - "); right[1] = buffer1;
    sprintf(buffer1, "%d %%", state.targetMaxHumidity); right[2] = buffer1;
    rightColors[0] = _config1FieldColor(humLow);
    rightColors[1] = GRAY_600;
    rightColors[2] = _config1FieldColor(humHigh);
    printTextFancy(3, left, leftColors, 1, right, rightColors, 3);

    left[0] = "  - Float";
    leftColors[0] = GRAY_600;
    sprintf(buffer1, "%d %%", state.humidityFloat); right[0] = buffer1;
    rightColors[0] = _config1FieldColor(humFloat);
    printTextFancy(4, left, leftColors, 1, right, rightColors, 1);

    left[0] = "Fan";
    leftColors[0] = GRAY_600;
    sprintf(buffer1, "%ds", state.fanDurationSeconds); right[0] = buffer1;
    sprintf(buffer1, " / "); right[1] = buffer1;
    sprintf(buffer1, "%dm", state.fanIntervalMinutes); right[2] = buffer1;
    rightColors[0] = _config1FieldColor(fanDuration);
    rightColors[1] = GRAY_600;
    rightColors[2] = _config1FieldColor(fanPeriod);
    printTextFancy(5, left, leftColors, 1, right, rightColors, 3);

    left[0] = "";
    leftColors[0] = GRAY_600;
    right[0] = "Exit";
    rightColors[0] = _config1FieldColor(Exit);
    printTextFancy(6, left, leftColors, 1, right, rightColors, 1);
  }
}

void onInputPress() {
  Serial.println("click");
  if (state.currentAppMode == MODE_CONFIG_1) {
    if (state.config1Field == Exit) {
      nextAppMode(MODE_IDLE);
    } else {
      state.editingCurrentField = !state.editingCurrentField;
    }
  } else {
    nextAppMode();
  }
  state.lastInputTime = millis();
}

bool fanOn = false;
void onInputLongPress() {
  if (state.currentAppMode == MODE_CONFIG_1) {
    state.editingCurrentField = false;
    nextAppMode(MODE_IDLE);
  }
  fanOn = !fanOn;
  analogWrite(FAN_PIN_1, fanOn ? 255: 0);
}

void onInputChange(int direction) {
  Serial.println("Encoder " + String(direction));
  state.lastInputTime = millis();
  if (state.currentAppMode == MODE_CONFIG_1) {
    if (state.editingCurrentField) {
      changeCurrentConfig1Field(direction);
    } else {
      nextConfig1EditField(direction);
    }
  }
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
