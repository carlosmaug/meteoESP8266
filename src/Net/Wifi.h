#ifndef DEBUG
#define DEBUG false 
#endif
#ifndef WIFIC_H
#define WIFIC_H
/*
   Wifi.h - Class to connect to wifi
*/
#include "WifiConfig.h"
#include <WiFiManager.h>

class Wifi {
public:
    static bool wifiFirstConnected;
    WiFiEventHandler event;
  
    Wifi();
    Wifi(char* apName, char* apPass);
    WiFiClient getClient();

    static void onWifiGotIp(WiFiEventStationModeGotIP ipInfo) {
        Wifi::wifiFirstConnected = true;

	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
    }
};

bool Wifi::wifiFirstConnected = false;

Wifi::Wifi() {
   Wifi(AP_NAME, AP_PASS);
}

Wifi::Wifi(char* apName, char* apPass) {
   WiFiManager *wifiManager;   

   // Connect to WIFI
   wifiManager = new WiFiManager();

   if (DEBUG) wifiManager->setDebugOutput(true);
 
   WiFi.onStationModeGotIP(Wifi::onWifiGotIp); 

   if (apName != NULL && apPass != NULL) 
       wifiManager->autoConnect(apName, apPass);
     
   wifiManager->setConfigPortalTimeout(30);

   Serial.println();
   Serial.print("Got IP: ");
   Serial.println(WiFi.localIP());
}

#endif
