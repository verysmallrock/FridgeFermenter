#include "SheetLogger.h"
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>

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

bool setupSheetLogger() {
  Serial.print("Logging data to ");
  Serial.println(host); 

  Serial.println("\nInitializing Sheet Header");
  client.get(initializeUrl);
  Serial.println("\nReady to log data");
}


void logTempHumidityToSheet(float temp, float humidity) {
  sheetTemp = String(temp);
  sheetHumid = String(humidity);

  String contentType = "application/json";
  payload = payload_base + "\"" + sheetTemp + "," + sheetHumid + "\"}";
  client.post(postUrl, contentType, payload);

  int statusCode = client.responseStatusCode();
  if(statusCode >= 400) {
    Serial.println("Sheet Logger response was " + statusCode);
  }
  
}
