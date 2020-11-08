#ifndef DEBUG
#define DEBUG false
#endif
#ifndef NTP_H
#define NTP_H

/*
   Ntp.h - Class to add ntp support
*/
#include <Time.h>
#include "NtpConfig.h"

class NtpClient {
public:
    NtpClient();
    timeStatus_t status(); 

private:
    void _init();
};

/**
 * Constructor
 */

NtpClient::NtpClient() {
    setSyncInterval(NTP_INTERVAL);
    configTime(TIMEZONE, DST_SEC, NTP_SERVER);
#ifdef WIFIC_H
    int i = 0;

    while(Wifi::wifiFirstConnected || i > 10) {
        delay(500);
        i++;
	Serial.println("Waiting WIFI to connect");
    };

    Wifi::wifiFirstConnected = false;
    _init();
#endif
}

/**
 * @brief Initiates NTP client
 */
void NtpClient::_init() {
    int    i   = 0;
    time_t now = time(NULL);
    struct tm timeinfo;

    if (DEBUG) Serial.print("Waiting for NTP time sync: ");
    
    while (now < 300 && i < 20) {
        if (DEBUG) Serial.print(".");
	Serial.print(now);
        delay(3000);
	now = time(NULL);
	i++;
    }

    if (i >= 100) 
	Serial.print("Error: NTP server not reachable.");

    if (DEBUG) {
	Serial.println("");
        gmtime_r(&now, &timeinfo);
    	Serial.print("Current time: ");
    	Serial.print(asctime(&timeinfo));
    }	
 }

/**
 * @brief Returns uptime information 
 */
timeStatus_t NtpClient::status() {
    return  timeStatus();
}

#endif
