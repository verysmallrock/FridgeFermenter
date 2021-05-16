#include "Main.h"
#include "AppState.h"
#include "Display.h"
#include "Power.h"
#include "SHT31.h"
#include "Wifi.h"
#include "SheetLogger.h"

SHT31 sht31 = SHT31();

void mainSetup() {
  int line = 1;
  loadAppState();
  setupDisplay();
  printTitle("Initializing...");

  setupPower();
  printText(line++, "Power setup OK");
  sht31.begin();
  printText(line++, "Temp setup OK");

  bool WifiOk = checkWifiConnection();
  printText(line++, WifiOk ? "Connected to Wifi" : "Wifi ERROR");

  bool sheetOk = setupSheetLogger();
  printText(line++, sheetOk ? "Sheetlogger OK": "Sheetlogger ERROR");
}

char buffer1[50];
char buffer2[50];

// This code assumes readSensors is called frequently to update currentTemp/currentHumidity
// All other tasks use those values for functionality.
float tempAlpha = 0.4;
float humidityAlpha = 0.07; // has more jitter, so lower alpha
int readErrorCount = 0;
void readSensors() {
  float currentTemp = sht31.convertCtoF(sht31.getTemperature());
  float currentHumidity = sht31.getHumidity();

  if (isnan(currentHumidity) || isnan(currentTemp)) {
    Serial.println("Failed to read from SHT sensor.");
    ++readErrorCount;
    if (readErrorCount == 20) {
      Serial.println("Unhealthy sensor read count.  Resetting sensor library.");
      SHT31 sht31 = SHT31();
      sht31.begin();
    }
    if (readErrorCount > 40) {
      Serial.println("Too many sensor read errors.  Resetting system.");
      NVIC_SystemReset();
    }
  } else {
    if(state.currentHumidity == 0) {
      state.currentHumidity = currentHumidity;
    } else {
      state.currentHumidity = (humidityAlpha * currentHumidity) + (1.0 - humidityAlpha) * state.currentHumidity;
    }

    if (state.currentTemp == 0) {
      state.currentTemp = currentTemp;
    } else {
      state.currentTemp = (tempAlpha * currentTemp) + (1.0 - tempAlpha) * state.currentTemp;
    }
    readErrorCount = 0;
    validateMinMaxTemp();
  }
}

String getTempActivityString() {
  if (state.heatingActive)
    return "Heat";
  else if (state.coolingActive)
    return "Cool";
  else return "Idle";
}

String getHumidityActivityString() {
  if (state.humidActive)
    return "Humidify";
  else if (state.dehumidActive)
    return "Dehumidify";
  else return "Idle";
}

void initializeMinMax(float currentTemp, float currentHumidity) {
  int floatRange = int(state.tempFloat * 1.8);
  state.minTemp = state.targetMinTemp - floatRange;
  state.maxTemp = state.targetMaxTemp + floatRange;

  int dehumidityFloatRange = int(state.dehumidityFloat * 1.8);
  int humidityFloatRange = int(state.humidityFloat * 1.8);
  state.minHumidity = state.targetMinHumidity - dehumidityFloatRange;
  state.maxHumidity = state.targetMaxHumidity + humidityFloatRange;

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
}

void appModeChanged() {
  if (state.currentAppMode == MODE_CONFIG_1 || state.currentAppMode == MODE_CONFIG_2) {
    // turn things off while editing
    activateHeat(POWER_OFF);
    activateFridge(POWER_OFF);
    activateHumidifier(POWER_OFF);
    activateDehumidifier(POWER_OFF);
    state.tempDirection = INACTIVE;
    state.humidityDirection = INACTIVE;
  }
  updateDisplay(true, false, false);

  minMaxInitialized = false;
  validateMinMaxTemp();
  drawDisplay(true);
}

uint16_t _getFieldColor(bool active) {
  if (active) {
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

uint16_t _config1FieldColor(Config1CurrentEditField field) {
  return _getFieldColor(int(state.config1Field) == int(field));
}

uint16_t _config2FieldColor(Config2CurrentEditField field) {
  return _getFieldColor(int(state.config2Field) == int(field));
}

void drawDisplay(bool drawNextPoint) {
  String left[5];
  uint16_t leftColors[5];
  String right[5];
  uint16_t rightColors[5];

  if (state.currentAppMode == MODE_IDLE) {
    updateDisplay(false, false, true);
    setGraphBottomRightStr(2, getTempActivityString());
    setGraphBottomRightStr(5, getHumidityActivityString());

    printTitle("Aging Box");
    if (state.lastHttpResponse != 0) {
      printTopRightStatus(String(state.lastHttpResponse));
    }
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

    left[0] = "Temp";
    leftColors[0] = GRAY_600;
    sprintf(buffer1, "%d", state.targetMinTemp); right[0] = buffer1;
    sprintf(buffer1, " - "); right[1] = buffer1;
    sprintf(buffer1, "%d F", state.targetMaxTemp); right[2] = buffer1;
    rightColors[0] = _config1FieldColor(C1TempLow);
    rightColors[1] = GRAY_600;
    rightColors[2] = _config1FieldColor(C1TempHigh);
    printTextFancy(1, left, leftColors, 1, right, rightColors, 3);

    left[0] = "  - Float";
    leftColors[0] = GRAY_600;
    sprintf(buffer1, "%d F", state.tempFloat); right[0] = buffer1;
    rightColors[0] = _config1FieldColor(C1TempFloat);
    printTextFancy(2, left, leftColors, 1, right, rightColors, 1);

    left[0] = "Humidity";
    leftColors[0] = GRAY_600;
    sprintf(buffer1, "%d", state.targetMinHumidity); right[0] = buffer1;
    sprintf(buffer1, " - "); right[1] = buffer1;
    sprintf(buffer1, "%d %%", state.targetMaxHumidity); right[2] = buffer1;
    rightColors[0] = _config1FieldColor(C1HumLow);
    rightColors[1] = GRAY_600;
    rightColors[2] = _config1FieldColor(C1HumHigh);
    printTextFancy(3, left, leftColors, 1, right, rightColors, 3);

    left[0] = "  - Float";
    leftColors[0] = GRAY_600;

    sprintf(buffer1, "%d %%", state.dehumidityFloat); right[0] = buffer1;
    sprintf(buffer1, ", "); right[1] = buffer1;
    sprintf(buffer1, "%d %%", state.humidityFloat); right[2] = buffer1;
    rightColors[0] = _config1FieldColor(C1DehumFloat);
    rightColors[1] = GRAY_600;
    rightColors[2] = _config1FieldColor(C1HumFloat);
    printTextFancy(4, left, leftColors, 1, right, rightColors, 3);

    left[0] = "Next";
    leftColors[0] = _config1FieldColor(C1Next);
    right[0] = "Exit";
    rightColors[0] = _config1FieldColor(C1Exit);
    printTextFancy(6, left, leftColors, 1, right, rightColors, 1);
  } else if (state.currentAppMode == MODE_CONFIG_2) {
    updateDisplay(false, false, true);

    printTitle("Configuration");

    left[0] = "Hume if Cool"; // run humidifier 100% if cooling
    leftColors[0] = GRAY_600;
    sprintf(buffer1, "%s", state.humidifyWhenCooling == 1 ? "Yes" : "No"); right[0] = buffer1;
    rightColors[0] = _config2FieldColor(C2HumWhenCooling);
    printTextFancy(1, left, leftColors, 1, right, rightColors, 1);

    left[0] = "Hume Cycle";
    leftColors[0] = GRAY_600;
    sprintf(buffer1, "%ds", state.humidityPeriod); right[0] = buffer1;
    sprintf(buffer1, " / "); right[1] = buffer1;
    sprintf(buffer1, "%ds", state.humidityBreak); right[2] = buffer1;
    rightColors[0] = _config2FieldColor(C2HumPeriod);
    rightColors[1] = GRAY_600;
    rightColors[2] = _config2FieldColor(C2HumBreak);
    printTextFancy(2, left, leftColors, 1, right, rightColors, 3);

    left[0] = "Ex Fan";
    leftColors[0] = GRAY_600;
    sprintf(buffer1, "%ds", state.fanDurationSeconds); right[0] = buffer1;
    sprintf(buffer1, " / "); right[1] = buffer1;
    sprintf(buffer1, "%dm", state.fanIntervalMinutes); right[2] = buffer1;
    rightColors[0] = _config2FieldColor(C2FanDuration);
    rightColors[1] = GRAY_600;
    rightColors[2] = _config2FieldColor(C2FanPeriod);
    printTextFancy(3, left, leftColors, 1, right, rightColors, 3);

    left[0] = "Int Fan";
    leftColors[0] = GRAY_600;
    sprintf(buffer1, "%ds", state.internalFanDurationSeconds); right[0] = buffer1;
    sprintf(buffer1, " / "); right[1] = buffer1;
    sprintf(buffer1, "%dm", state.internalFanIntervalMinutes); right[2] = buffer1;
    rightColors[0] = _config2FieldColor(C2IntFanDuration);
    rightColors[1] = GRAY_600;
    rightColors[2] = _config2FieldColor(C2IntFanPeriod);
    printTextFancy(4, left, leftColors, 1, right, rightColors, 3);

    left[0] = "Log To Cloud";
    leftColors[0] = GRAY_600;
    sprintf(buffer1, "%s", state.logDataToCloud == 1 ? "Yes" : "No"); right[0] = buffer1;
    rightColors[0] = _config2FieldColor(C2LogDataToCloud);
    printTextFancy(5, left, leftColors, 1, right, rightColors, 1);

    left[0] = "";
    leftColors[0] = GRAY_600;
    right[0] = "Exit";
    rightColors[0] = _config2FieldColor(C2Exit);
    printTextFancy(6, left, leftColors, 1, right, rightColors, 1);
  }
}

void onInputPress() {
  Serial.println("click");
  if (state.currentAppMode == MODE_CONFIG_1) {
    if (state.config1Field == C1Exit) {
      nextAppMode(MODE_IDLE);
    } else if (state.config1Field == C1Next) {
      nextAppMode(MODE_CONFIG_2);
    } else {
      state.editingCurrentField = !state.editingCurrentField;
    }
  } else if (state.currentAppMode == MODE_CONFIG_2) {
    if (state.config2Field == C2Exit) {
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
  if (state.currentAppMode == MODE_CONFIG_1 || state.currentAppMode == MODE_CONFIG_2) {
    state.editingCurrentField = false;
    nextAppMode(MODE_IDLE);
  }
}

void onInputChange(int direction) {
  Serial.println("Encoder " + String(direction));
  state.lastInputTime = millis();
  if (state.editingCurrentField) {
    changeCurrentConfigField(direction);
  } else {
    nextConfigEditField(direction);
  }
}

bool toggle = false;

void updateRelays() {
  if (state.currentAppMode != MODE_IDLE || state.currentTemp <= 0) { return; }

  // Temperature checks
  int allowedMinTemp = state.targetMinTemp;
  if (state.tempDirection == DECREASE)
    allowedMinTemp -= state.tempFloat;
  int allowedMaxTemp = state.targetMaxTemp;
  if (state.tempDirection == INCREASE)
    allowedMaxTemp += state.tempFloat;

  if (state.currentTemp < allowedMinTemp) {
    activateFridge(POWER_OFF);
  } else if (state.currentTemp > allowedMaxTemp)  {
    activateFridge(POWER_ON);
  }

  if (state.currentTemp < allowedMinTemp){
    activateHeat(POWER_ON);
  } else if (state.currentTemp > allowedMaxTemp)  {
    activateHeat(POWER_OFF);
  }

  // Decide when to turn temperature control off
  if (state.tempDirection == DECREASE && state.currentTemp < state.targetMinTemp) {
    activateFridge(POWER_OFF);
  }
  else if (state.tempDirection == INCREASE && state.currentTemp > state.targetMaxTemp) {
    activateHeat(POWER_OFF);
  }

  // Humidity checks
  int allowedMinHumidity = state.targetMinHumidity;
  if (state.humidityDirection == DECREASE)
    allowedMinHumidity -= state.dehumidityFloat;
  int allowedMaxHumidity = state.targetMaxHumidity;
  if (state.humidityDirection == INCREASE)
    allowedMaxHumidity += state.humidityFloat;

  // Note the allowedMaxTemp check here -- Don't humidify if we're way over intended humidity.
  if (state.humidifyWhenCooling == 1 && state.coolingActive && state.currentHumidity < state.targetMaxHumidity) {
    // humidify if cooling
    activateHumidifier(POWER_ON);
    activateDehumidifier(POWER_OFF); // ensure De-hume power off!
    state.humidityDirection = INACTIVE; // ignore float logic
  } else {
    if (state.currentHumidity < allowedMinHumidity) {
      activateDehumidifier(POWER_OFF);
    } else if (state.currentHumidity > allowedMaxHumidity)  {
      activateDehumidifier(POWER_ON);
    }

    if (state.currentHumidity < allowedMinHumidity) {
      if(millis() % ((state.humidityPeriod + state.humidityBreak) * 1000) < (state.humidityPeriod * 1000))
        activateHumidifier(POWER_ON);
      else
        activateHumidifier(POWER_OFF, false);
    } else if (state.currentHumidity > allowedMaxHumidity)  {
      activateHumidifier(POWER_OFF);
    }

    // Decide when to turn humidity control off
    if (state.humidityDirection == DECREASE && state.currentHumidity < state.targetMinHumidity) {
      activateDehumidifier(POWER_OFF);
    }
    else if (state.humidityDirection == INCREASE && state.currentHumidity > state.targetMaxHumidity) {
      activateHumidifier(POWER_OFF);
    }
  }
}

void updateFans() {
  if (state.airExchangeActive) {
    if (millis() - state.lastAirExchangeUpdate > (state.fanDurationSeconds * 1000)) {
      Serial.println("Deactivating air exchange fans");
      analogWrite(FAN_PIN_1, 0);
      state.airExchangeActive = false;
      state.lastAirExchangeUpdate = millis();
    }

  } else {
    if (millis() - state.lastAirExchangeUpdate > (state.fanIntervalMinutes * 60 * 1000)) {
      Serial.println("Activating air exchange fans");
      analogWrite(FAN_PIN_1, 255);
      state.airExchangeActive = true;
      state.lastAirExchangeUpdate = millis();
    }
  }

  if (state.internalFanActive) {
    if (millis() - state.lastInternalFanUpdate > (state.internalFanDurationSeconds * 1000)) {
      Serial.println("Deactivating Internal Fans");
      analogWrite(FAN_PIN_2, 0);
      state.internalFanActive = false;
      state.lastInternalFanUpdate = millis();
    }

  } else {
    if (millis() - state.lastInternalFanUpdate > (state.internalFanIntervalMinutes * 60 * 1000)) {
      Serial.println("Activating Internal Fans");
      analogWrite(FAN_PIN_2, 255);
      state.internalFanActive = true;
      state.lastInternalFanUpdate = millis();
    }
  }
}

void logSensorsToCloud() {
  logTempHumidityToSheet(state.currentTemp, state.currentHumidity);
}
