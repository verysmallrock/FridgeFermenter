# Mini-Fridge Fermenter Code

Arduio Nano 33 IOT code to power a mini-fridge for sausage and cheese fermentation.

Components:

- 120V AC Mini-fridge
- 120V AC heating element
- 120V AC Humidifier (120V connected to DC Adapter)
- 120V AC De-humidifier (120V connected to DC Adapter)
- 12V fans (powered by 5V USB power)


## Config

Create a `Secrets.h` file that looks like this:

```
#ifndef SSID
#define SSID "WIFISSD"
#define PASSWORD "WIFIPASSWORD"
#define SECRET_GSCRIPT_ID "GOOGLESHEET SCRIPT ID"
#endif

```

TBD Details on the Google sheet.
