#include <vector>
#include "src/config.h"
#include "src/Net/Wifi.h"
#include "src/Net/Ntp.h"
#include "src/Net/Rest.h"
#include "src/Web/Web.h"
#include "src/Web/DisplaySensorData.h"
#include "src/Sensores/Dht.h"
#include "src/Sensores/Bmp180.h"
#include "src/Sensores/Bh1750.h"
#include "src/Sensores/Veml6070.h"
#include "src/Fs/Config.h"

Wifi          *wifi;
NtpClient     *ntpClient;
EspRestClient *rest;

char       espNameC[50];
//std::vector <sensor> sensors;

//------------------------------------------
//---- Sensors and other initialitations ---
//------------------------------------------
Wifi      *net;
//Config     *config;
DisplaySensorData  *www;
Dht22     *dht;
Bmp180    *bmp;
Bh1750    *bh;
Veml6070  *veml;

//-------------------------------------------------------------------------------

unsigned long interval;

void setup() {
    // Start Serial for debugging on the Serial Monitor
    Serial.begin(115200);
    
    net       = new Wifi();
    ntpClient = new NtpClient();
    www       = new DisplaySensorData();
    bmp       = new Bmp180(sensors);
    dht       = new Dht22(sensors);
    bh        = new Bh1750(sensors);
    veml      = new Veml6070(sensors);
    
    rest        = new EspRestClient();
    //config    = new Config();
    
    interval    = 0;

}

void loop() {

  if (millis() > interval) {
    interval = millis() + 1000 * readInterval;
    
    bmp->read(sensors);
    dht->read(sensors);
    bh->read(sensors);
    veml->read(sensors);
  
    // Update web data
    webSensors();
    
    www->updateSensorData();
    
    publishValues();
    
    Serial.println(ntpClient->status());
  }

  www->read();
}

/**
 * Stablish sensor name, unit, image and data to be rendered into the web
 */
void webSensors() {
   for (int i = 0; i < sensors.size(); i++) {
       if (sensors[i].name != "") {
           www->datos[i].name = sensors[i].name;
           www->datos[i].unit = sensors[i].unit;
           www->datos[i].img  = sensors[i].img;
           www->datos[i].data = sensors[i].data;
           www->datos[i].min  = sensors[i].min;
           www->datos[i].max  = sensors[i].max;
       }
   }    
}

// Rest publish values
void publishValues () {
    String name;

    for (int i = 0; i < NUMSENSORS; i++) {
        if (www->datos[i].name != "") {
            name = www->datos[i].name;
     
            Serial.println(name);
            
            if (isnan(www->datos[i].data)) www->datos[i].data = 0.00;
            
            Serial.println(www->datos[i].data);
            rest->sendData(name, www->datos[i].data);
        }
    }
}
