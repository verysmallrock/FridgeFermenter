#include "Wifi.h"
#include "AppState.h"

int status = WL_IDLE_STATUS;

void preflight() {
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

bool checkWifiConnection() {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  } else {
    if (status == WL_CONNECTED) {
      Serial.println("Wifi was disconnected.  Reconnecting...");
    }
    status = WL_IDLE_STATUS;
  }
  Serial.println("Connecting to " + String(SSID));

  preflight();

  // attempt to connect to WiFi network:
  int maxRetries = 30;
  while (status != WL_CONNECTED) {
    Serial.println("Attempting to connect to WPA SSID: ");
    Serial.println(SSID);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(SSID, PASSWORD);
    --maxRetries;
    if (maxRetries <= 0) {
      // Something has gone wrong.  Soft reset.
      NVIC_SystemReset();
    }

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
