#include "Main.h"
#include "Display.h"
#include "Power.h"
#include "DHT.h"
#include "Wifi.h"
#include "SheetLogger.h"

int currentAppMode = MODE_IDLE;

#define DEBUG false

#define TEMPHUMIDITY_PIN A7
DHT dht(TEMPHUMIDITY_PIN, DHT22);
float temp;
float humidity;

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

// This code assumes readSensors is called frequently to update temp/humidity
// All other tasks use those values for functionality.
void readSensors() {
  temp = dht.convertCtoF(dht.readTemperature());
  humidity = dht.readHumidity();
  if (DEBUG) {
    Serial.println("Temp: " + String(temp) + " Humidity: " + String(humidity));
  }
  validateMinMaxTemp();
}

char buffer1[50];
char buffer2[50];

int minTemp = 40;
int maxTemp = 90;
int targetMinTemp = 65;
int targetMaxTemp = 70;
int tempFloat = 3;
int tempWidth = 10;

int minHumidity = 55;
int maxHumidity = 90;
int targetMinHumidity = 70;
int targetMaxHumidity = 75;
int humidityFloat = 3;
int humidityWidth = 10;



void initializeMinMax(float temp, float humidity) {
  minTemp = int(temp) - tempWidth;
  maxTemp = int(temp) + tempWidth;
  minHumidity = int(humidity) - humidityWidth;
  maxHumidity = int(humidity) + humidityWidth;

  if (currentAppMode == MODE_IDLE) {
    sprintf(buffer1, "%d F", minTemp);
    sprintf(buffer2, "%d F", maxTemp);
    printGraphBg(2, buffer1, buffer2);
    sprintf(buffer1, "%d%%", minHumidity);
    sprintf(buffer2, "%d%%", maxHumidity);
    printGraphBg(5, buffer1, buffer2);
  }
}

bool minMaxInitialized = false;
void validateMinMaxTemp() {
  if (!minMaxInitialized) {
    initializeMinMax(temp, humidity);
    minMaxInitialized = true;
  }
  if (isnan(humidity) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor. Setting to 50/50.");
    temp = humidity = 50;
  }
}

void appModeChanged() {
  updateDisplay(true, false, false);

  minMaxInitialized = false;
  validateMinMaxTemp();
}

void drawDisplay() {
  if (currentAppMode == MODE_IDLE) {
    updateDisplay(false, false, true);

    printTitle("Aging Box 0.1");
    sprintf(buffer1, "%0.1f F", temp);
    printText(1, "Temperature", buffer1);
    printNextGraphPoint(2, (temp - minTemp) / (maxTemp - minTemp));

    sprintf(buffer1, "%0.1f %%", humidity);
    printText(4, "Humidity", buffer1);
    printNextGraphPoint(5, (humidity - minHumidity) / (maxHumidity - minHumidity));
  } else if (currentAppMode == MODE_CONFIG_1) {
    updateDisplay(false, false, true);

    printTitle("Configuration");
    sprintf(buffer1, "%d - %d F", targetMinTemp, targetMaxTemp);
    printText(1, "Temp", buffer1);

    sprintf(buffer1, "%d", tempFloat);
    printText(2, "  - Float", buffer1);

    sprintf(buffer1, "%d - %d %%", targetMinHumidity, targetMaxHumidity);
    printText(3, "Humidity", buffer1);

    sprintf(buffer1, "%d", humidityFloat);
    printText(4, "  - Float", buffer1);
  }
}

bool toggle = false;
void updateRelays() {
  //toggle = !toggle;
  activateHumidifier(toggle);
  activateDehumidifier(toggle);
  activateFridge(toggle);
  activateHeat(toggle);
}

void logSensorsToCloud() {
  logTempHumidityToSheet(temp, humidity);
}
