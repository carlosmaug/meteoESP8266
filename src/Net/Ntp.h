#ifndef DEBUG
#define DEBUG false
#endif
#ifndef NTP_H
#define NTP_H

/*
   Ntp.h - Class to add ntp support
*/
#include <NtpClientLib.h>

class NtpClient {
public:
    static NTPSyncEvent_t ntpLastEvent; // Last triggered event
    static bool syncEventTriggered;     // True if a time even has been triggered

    NtpClient();
    void ntpSyncEvent(); 
    String status(); 

private:
    void _init();
};

bool           NtpClient::syncEventTriggered = false;   
NTPSyncEvent_t NtpClient::ntpLastEvent;

/**
 * Constructor
 */

NtpClient::NtpClient() {
#ifdef WIFIC_H
    int i = 0;

    do {
        delay(500);
        i++;
    } while(Wifi::wifiFirstConnected || i > 10);

    Wifi::wifiFirstConnected = false;
    _init();
#endif
}

/**
 * @brief Initiates NTP client
 */
void NtpClient::_init() {
    // NTP define events
    NTP.onNTPSyncEvent([](NTPSyncEvent_t event) {
       	NtpClient::ntpLastEvent       = event;
        NtpClient::syncEventTriggered = true;
    });

    NTP.setInterval(ntpint);
    NTP.begin(ntpServer, 1, true);
}

/**
 * @brief This function is called when an NTP syncronization event is trigered
 */
void NtpClient::ntpSyncEvent() {
    if (NtpClient::syncEventTriggered) {
        if (NtpClient::ntpLastEvent < 0) {
            Serial.printf ("Time Sync error: %d\n", NtpClient::ntpLastEvent);

            if (NtpClient::ntpLastEvent == noResponse) {
                Serial.println ("NTP server not reachable");
            } else if (NtpClient::ntpLastEvent == invalidAddress) {
                Serial.println ("Invalid NTP server address");
	    } else if (NtpClient::ntpLastEvent == errorSending) {
                Serial.println ("Error sending request");
            } else if (NtpClient::ntpLastEvent == responseError) {
                Serial.println ("NTP response error");
            }
	} else {
            if (NtpClient::ntpLastEvent == timeSyncd && DEBUG) {
                Serial.print ("Got NTP time: ");
                Serial.println (NTP.getTimeDateString (NTP.getLastNTPSync()));
            }
        }

        NtpClient::syncEventTriggered = false;
    }
}

/**
 * @brief Returns uptime information 
 */
String NtpClient::status() {
     return NTP.getTimeDateString() + "  Uptime: " + NTP.getUptimeString() + " Since " + NTP.getTimeDateString(NTP.getFirstSync()).c_str();
}

#endif
