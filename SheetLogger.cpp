#include "Wifi.h"
#include "SheetLogger.h"
#include "AppState.h"
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include "Secrets.h"

String sheetHumid = "";
String sheetTemp = "";

const char* host = "script.google.com";

String initializeUrl = String("/macros/s/") + GSCRIPTID + "/exec?value=Temperature";  // Write Temperature to Google Spreadsheet at cell A1
// Fetch Google Calendar events for 1 week ahead
String postUrl = String("/macros/s/") + GSCRIPTID + "/exec?cal";  // Write to Cell A continuosly

//replace with sheet name not with spreadsheet file name taken from google
String payload_base =  "{\"command\": \"appendRow\", \
                    \"sheet_name\": \"TempSheet\", \
                       \"values\": ";
String payload = "";

WiFiSSLClient wifi;
HttpClient client = HttpClient(wifi, host, 443);

int clientGet(String path) {
  Serial.println("GET " + String(host) + path);
  client.get(path);
  int statusCode = client.responseStatusCode();
  return statusCode;
}

int clientPost(String path, String contentType, String body) {
  bool status = checkWifiConnection();
  if (!status) {
    Serial.println("Waiting to POST.");
    return 0;
  }
  IPAddress ip = WiFi.localIP();

  client.beginRequest();
  client.post(path);
  client.sendHeader("Content-Type",contentType);
  client.sendHeader("Content-Length", body.length());
  client.beginBody();
  client.print(body);
  client.endRequest();

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  Serial.println("POST: " + String(statusCode));
  return statusCode;
}

bool setupSheetLogger() {
  if (!state.logDataToCloud) {
    Serial.println("Data logging is switched off. Skipping Sheet setup.");
    return 0;
  }
  Serial.println("Logging data to " + String(host));

  Serial.println("\nInitializing Sheet Header");
  int statusCode = clientGet(initializeUrl);
  state.lastHttpResponse = statusCode;
  Serial.println(statusCode);
  Serial.println("Ready to log data");
}

int trueScale = 70;
void logTempHumidityToSheet(float temp, float humidity) {
  if (!state.logDataToCloud) {
    Serial.println("Data logging is switched off. Skipping Log request.");
    return;
  }
  sheetTemp = String(temp);
  sheetHumid = String(humidity);

  String contentType = "application/json";
  payload = payload_base + "\"" + sheetTemp + "," + sheetHumid;
  payload = payload + "," + state.coolingActive * trueScale + "," + state.heatingActive * trueScale;
  payload = payload + "," + state.dehumidActive * trueScale + "," + state.humidActive * trueScale;
  payload = payload + "," + state.airExchangeActive * trueScale + "," + state.internalFanActive * trueScale;
  payload = payload + "\"}";
  Serial.println("Logging state to sheet...");
  int statusCode = clientPost(postUrl, contentType, payload);
  state.lastHttpResponse = statusCode;

  if(statusCode >= 400 || statusCode < 200) {
    Serial.println("Sheet Logger response was " + statusCode);
  }
  Serial.println("POST finished.\n");
}
