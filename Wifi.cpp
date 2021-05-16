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
  status = WiFi.status();
  if (status == WL_CONNECTED) {
    return true;
  } else {
    if (status == WL_CONNECTED) {
      // https://github.com/arduino-libraries/WiFiNINA/issues/103
      // also https://forum.arduino.cc/t/wifi-not-stable-over-longer-periods-is-this-normal-ideas/636846/41
      Serial.println("Wifi was disconnected.  Reconnecting...");
    }
  }
  Serial.println("Connecting to " + String(SSID));

  preflight();

  // attempt to connect to WiFi network:
  int maxRetries = 30;
  while (status != WL_CONNECTED) {
    --maxRetries;
    if (maxRetries <= 0) {
      Serial.println("Too many retries. Resetting system.");
      // Something has gone wrong.  Soft reset.
      NVIC_SystemReset();
      return false;
    }
    Serial.println("Attempting to connect to WPA SSID: ");
    Serial.println(SSID);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(SSID, PASSWORD);
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
