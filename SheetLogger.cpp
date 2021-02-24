#include "HTTPSRedirect.h"
#include "SheetLogger.h"

float h;
float t;
String sheetHumid = "";
String sheetTemp = "";

const char* host = "script.google.com";

const int httpsPort = 443; //the https port is same

String url = String("/macros/s/") + GSCRIPTID + "/exec?value=Temperature";  // Write Temperature to Google Spreadsheet at cell A1
// Fetch Google Calendar events for 1 week ahead
String url2 = String("/macros/s/") + GSCRIPTID + "/exec?cal";  // Write to Cell A continuosly

//replace with sheet name not with spreadsheet file name taken from google
String payload_base =  "{\"command\": \"appendRow\", \
                    \"sheet_name\": \"TempSheet\", \
                       \"values\": ";
String payload = "";

HTTPSRedirect* client = nullptr;


bool setupSheetLogger() {
  // Use HTTPSRedirect class to create a new TLS connection
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  Serial.print("Connecting to ");
  Serial.println(host);          //try to connect with "script.google.com"

  // Try to connect for a maximum of 5 times then exit
  bool flag = false;
  for (int i = 0; i < 5; i++) {
    int retval = client->connect(host, httpsPort);
    if (retval == 1) {
      flag = true;
      break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }

  if (!flag) {
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    return false;
  }
  // Finish setup() function in 1s since it will fire watchdog timer and will reset the chip.
  //So avoid too many requests in setup()

  Serial.println("\nWrite into cell 'A1'");
  Serial.println("------>");
  // fetch spreadsheet data
  client->GET(url, host);

  Serial.println("\nGET: Fetch Google Calendar Data:");
  Serial.println("------>");
  // fetch spreadsheet data
  client->GET(url2, host);

 Serial.println("\nStart Sending Sensor Data to Google Spreadsheet");


  // delete HTTPSRedirect object
  delete client;
  client = nullptr;
  return true;
}


void logTempHumidityToSheet(float temp, float humidity) {
  sheetTemp = String(temp);
  sheetHumid = String(humidity);

  static int error_count = 0;
  static int connect_count = 0;
  const unsigned int MAX_CONNECT = 20;
  static bool flag = false;

  payload = payload_base + "\"" + sheetTemp + "," + sheetHumid + "\"}";

  if (!flag) {
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    flag = true;
    client->setPrintResponseBody(true);
    client->setContentTypeHeader("application/json");
  }

  if (client != nullptr) {
    if (!client->connected()) {
      client->connect(host, httpsPort);
      client->POST(url2, host, payload, false);
    }
  }
  else {
    Serial.println("Error creating client object!");
  }

  if (connect_count > MAX_CONNECT) {
    connect_count = 0;
    flag = false;
    delete client;
    return;
  }

  if (client->POST(url2, host, payload)) {
    ;
  }
  else {
    Serial.println("Error sending data.");
  }
}
