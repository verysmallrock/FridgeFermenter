#include "Wifi.h"

int status = WL_IDLE_STATUS; 

void preflight() {
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware (" + fv + " < " + String(WIFI_FIRMWARE_LATEST_VERSION) + ")");
  }
}

bool connectToWifi() {
  Serial.print("Connecting to " + String(SSID));

  preflight();

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(SSID);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(SSID, PASSWORD);

    delay(3000);
  }
  

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  return true;
}

long getSignalStrength() {
  long rssi = WiFi.RSSI();
  return rssi;
}
