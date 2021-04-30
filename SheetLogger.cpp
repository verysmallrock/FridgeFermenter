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


int clientGet(String path) {
  HttpClient client = HttpClient(wifi, host, 443);
  Serial.println("GET " + String(host) + path);
  client.get(path);
  int statusCode = client.responseStatusCode();
  //String response = client.responseBody();
  return statusCode;
}

int clientPost(String path, String contentType, String body) {
  HttpClient client = HttpClient(wifi, host, 443);
  IPAddress ip = WiFi.localIP();
  Serial.println("POST " + String(host) + path + ": " + body);
  Serial.println(ip);
  client.post(path, contentType, body);
  int statusCode = client.responseStatusCode();

  client.flush();
  return statusCode;
}

bool setupSheetLogger() {
  Serial.print("Logging data to ");
  Serial.println(host);

  Serial.println("\nInitializing Sheet Header");
  clientGet(initializeUrl);
  Serial.println("\nReady to log data");
}

int trueScale = 70;
void logTempHumidityToSheet(float temp, float humidity) {
  sheetTemp = String(temp);
  sheetHumid = String(humidity);

  String contentType = "application/json";
  payload = payload_base + "\"" + sheetTemp + "," + sheetHumid;
  payload = payload + "," + state.coolingActive * trueScale + "," + state.heatingActive * trueScale;
  payload = payload + "," + state.dehumidActive * trueScale + "," + state.humidActive * trueScale;
  payload = payload + "," + state.airExchangeActive * trueScale + "," + state.internalFanActive * trueScale;
  payload = payload + "\"}";
  int statusCode = clientPost(postUrl, contentType, payload);

  if(statusCode >= 400) {
    Serial.println("Sheet Logger response was " + statusCode);
  }

  Serial.println("logged temp " + String(statusCode));
}
