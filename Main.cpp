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
  loadAppState();
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
  drawDisplay(true);
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

void drawDisplay(bool drawNextPoint) {
  if (state.currentAppMode == MODE_IDLE) {
    updateDisplay(false, false, true);
    setGraphBottomRightStr(2, getTempActivityString());
    setGraphBottomRightStr(5, getHumidityActivityString());

    printTitle("Aging Box");
    sprintf(buffer1, "%0.1f F", state.currentTemp);
    printText(1, "Temperature", buffer1);
    if (drawNextPoint)
      printNextGraphPoint(2, (state.currentTemp - state.minTemp) / (state.maxTemp - state.minTemp));

    sprintf(buffer1, "%0.1f %%", state.currentHumidity);
    printText(4, "Humidity", buffer1);
    if (drawNextPoint)
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
    sprintf(buffer1, "%d F", state.tempFloat); right[0] = buffer1;
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
  if (state.currentAppMode != MODE_IDLE || state.currentTemp <= 0) { return; }

  int allowedMinTemp = state.targetMinTemp;
  if (state.tempDirection == DECREASE)
    allowedMinTemp -= state.tempFloat;
  int allowedMaxTemp = state.targetMaxTemp;
  if (state.tempDirection == INCREASE)
    allowedMaxTemp += state.tempFloat;

  if (state.currentTemp < allowedMinTemp){
    activateHeat(POWER_ON);
    activateFridge(POWER_OFF);
    state.tempDirection = INCREASE;
  } else if (state.currentTemp > allowedMaxTemp)  {
    activateHeat(POWER_OFF);
    activateFridge(POWER_ON);
    state.tempDirection = DECREASE;
  } else {
    activateHeat(POWER_OFF);
    activateFridge(POWER_OFF);
    state.tempDirection = INACTIVE;
  }

  int allowedMinHumidity = state.targetMinHumidity;
  if (state.humidityDirection == DECREASE)
    allowedMinHumidity -= state.humidityFloat;
  int allowedMaxHumidity = state.targetMaxHumidity;
  if (state.humidityDirection == INCREASE)
    allowedMaxHumidity += state.humidityFloat;

  if (state.currentHumidity < allowedMinHumidity){
    activateHumidifier(POWER_ON);
    activateDehumidifier(POWER_OFF);
    state.humidityDirection = INCREASE;
  } else if (state.currentHumidity > allowedMaxHumidity)  {
    activateHumidifier(POWER_OFF);
    activateDehumidifier(POWER_ON);
    state.humidityDirection = DECREASE;
  } else {
    activateHumidifier(POWER_OFF);
    activateDehumidifier(POWER_OFF);
    state.humidityDirection = INACTIVE;
  }
}

void updateFans() {
  if (state.fanActive) {
    if (millis() - state.lastFanUpdate > (state.fanDurationSeconds * 1000)) {
      Serial.println("Deactivating Fans");
      analogWrite(FAN_PIN_1, 0);
      state.fanActive = false;
      state.lastFanUpdate = millis();
    }

  } else {
    if (millis() - state.lastFanUpdate > (state.fanIntervalMinutes * 60 * 1000)) {
      Serial.println("Activating Fans");
      analogWrite(FAN_PIN_1, 255);
      state.fanActive = true;
      state.lastFanUpdate = millis();
    }

  }
}

void logSensorsToCloud() {
  logTempHumidityToSheet(state.currentTemp, state.currentHumidity);
}
