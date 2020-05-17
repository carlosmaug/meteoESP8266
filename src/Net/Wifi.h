#ifndef DEBUG
#define DEBUG false
#endif
#ifndef WIFIC_H
#define WIFIC_H
/*
   Wifi.h - Class to connect to wifi
*/
#include "WifiConfig.h"
#include <ESP8266WiFi.h>
#include <ESP_WiFiManager.h>

class Wifi {
public:
    static bool wifiFirstConnected;
    WiFiEventHandler event;
  
    Wifi();
    Wifi(char* apName, char* apPass);
    WiFiClient getClient();

    static void onWifiGotIp(WiFiEventStationModeGotIP ipInfo) {
        Wifi::wifiFirstConnected = true;
    }
};

bool Wifi::wifiFirstConnected = false;

Wifi::Wifi() {
   Wifi(AP_NAME, AP_PASS);
}

Wifi::Wifi(char* apName, char* apPass) {
   ESP_WiFiManager *wifiManager;   

   // Connect to WIFI
   wifiManager = new ESP_WiFiManager();

   if (apName != NULL && apPass != NULL) 
       wifiManager->autoConnect(apName, apPass);
     
   wifiManager->setConfigPortalTimeout(30);

   if (DEBUG) wifiManager->setDebugOutput(true);
 
   wifiManager->setDebugOutput(true);

   event = WiFi.onStationModeGotIP(Wifi::onWifiGotIp); 
}

#endif
