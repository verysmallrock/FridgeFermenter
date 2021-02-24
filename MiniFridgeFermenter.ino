/*
 * ESP8266 Variants on certain things (pins and such)
 * Started with this: https://chewett.co.uk/blog/1476/using-the-dht22-temperature-sensor-with-a-wemos-d1-mini-esp8266/
 */

#include "DHTesp.h"
#include "Wifi.h"
#include "SheetLogger.h"
#include "Display.h"
#include "Power.h"

DHTesp dht;
#define TEMPHUMIDITY_PIN D0

const int spinwheelClickPin = A0;
const int encoderAPin = 2;
const int encoderBPin = 3;
const int maximumEncoderValue = 128;

char buffer1[50];
char buffer2[50];

void setup() {
  Serial.begin(115200);
  setupPower();
  dht.setup(TEMPHUMIDITY_PIN, DHTesp::DHT22);
  setupDisplay();
  printTitle("Initializing...");

  bool WifiOk = connectToWifi();
  printText(1, WifiOk ? "Connected to Wifi" : "Wifi ERROR");

  bool sheetOk = setupSheetLogger();
  printText(2, sheetOk ? "Sheetlogger OK": "Sheetlogger ERROR");

  delay(2000);
  updateDisplay(true, false, false);
}

int minTemp = 40;
int maxTemp = 90;

int minHumidity = 55;
int maxHumidity = 90;
int tempWidth = 10;
int humidityWidth = 10;

void initializeMinMax(float temp, float humidity) {
  minTemp = int(temp) - tempWidth;
  maxTemp = int(temp) + tempWidth;
  minHumidity = int(humidity) - humidityWidth;
  maxHumidity = int(humidity) + humidityWidth;

  sprintf(buffer1, "%d F", minTemp);
  sprintf(buffer2, "%d F", maxTemp);
  printGraphBg(2, buffer1, buffer2);
  sprintf(buffer1, "%d%%", minHumidity);
  sprintf(buffer2, "%d%%", maxHumidity);
  printGraphBg(5, buffer1, buffer2);
}

boolean initialized = false;
bool toggle = false;
void loop() {
  float temp = DHTesp::toFahrenheit(dht.getTemperature());
  float humidity = dht.getHumidity();
  if (!initialized) {
    initializeMinMax(temp, humidity);
    initialized = true;
  }
  // toggle = !toggle;
  // activateHumidifier(toggle);
  // activateDehumidifier(toggle);
  // activateFridge(toggle);
  // activateHeat(toggle);

  logTempHumidityToSheet(temp, humidity);
  updateDisplay(false, false, true);

  printTitle("Aging Box 0.1");
  sprintf(buffer1, "%0.1f F", temp);
  printText(1, "Temperature", buffer1);
  printNextGraphPoint(2, (temp - minTemp) / (maxTemp - minTemp));

  sprintf(buffer1, "%0.1f %%", humidity);
  printText(4, "Humidity", buffer1);
  printNextGraphPoint(5, (humidity - minHumidity) / (maxHumidity - minHumidity));

  delay(100);
}
