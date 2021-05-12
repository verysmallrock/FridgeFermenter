#ifndef WIFI_INCLUDE
#define WIFI_INCLUDE

#include <SPI.h>
#include <WiFiNINA.h>
#include "Secrets.h"

bool checkWifiConnection();
long getSignalStrength();

#endif
