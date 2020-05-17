
#include <vector>
#include "config_prod.h"
#include "Net/Wifi.h"
#include "Net/Ntp.h"
//#include "Net/Mqtt.h"
#include "Web/Web.h"
#include "Web/Template.h"
#include "Sensores/Dht22.h"
#include "Sensores/Bmp180.h"
#include "Sensores/Bh1750.h"
#include "Sensores/Veml6070.h"

#include <ArduinoMqttClient.h>

Wifi      *wifi;
NtpClient *ntpClient;

WiFiClient wifiClient;
MqttClient mqtt(wifiClient);
char       espNameC[50];
std::vector <sensor> sensors;

//------------------------------------------
//---- Sensors and other initialitations ---
//------------------------------------------
Wifi      *net;
//Mqtt      *mqtt;
Template  *www;
Dht22     *dht;
Bmp180    *bmp;
Bh1750    *bh;
Veml6070  *veml;

//-------------------------------------------------------------------------------

unsigned long interval;

void setup() {
    // Start Serial for debugging on the Serial Monitor
    Serial.begin(115200);
    
    net       = new Wifi(apName, apPass);
    ntpClient = new NtpClient();
    www       = new Template();
    bmp       = new Bmp180(sensors);
    dht       = new Dht22(sensors);
    bh        = new Bh1750(sensors);
    veml      = new Veml6070(sensors);
    interval  = 0;

    webSensors();
   
    //mqtt      = new Mqtt();
    mqttInit();
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
    
    www->updateRootTemplate();
    
    mqttPublishValues();
    
    Serial.println(ntpClient->status());
  }

  ntpClient->ntpSyncEvent();
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


//MQTT publish values
void mqttPublishValues () {
    mqttReconnect();
    String topic;

    for (int i = 0; i < NUMSENSORS; i++) {
        if (www->datos[i].name != "") {
            topic = www->datos[i].name;
            
            topic.replace(" ", "");
            Serial.println(topic);
            mqttSendMessage(topic, www->datos[i].data);
        }
    }
}

//------------------------------- MQTT Stuff --------------------


void mqttInit() {
    mqttReconnect();
    setEspName("Meteo_1");
   //mqtt->setCallback(Mqtt::callback);

    mqttBaseTopic = mqttBaseTopic + "/" + espName;
}

void setEspName(String espname) {
    espName = espname;
    espname.toCharArray(espNameC, espname.length() + 1);
}


void mqttReconnect() {
    // Loop until we're reconnected
    int i                   = 0;
    const char mqttServer[] = "10.0.1.3";
    int mqttPort            = 1883;

    if (DEBUG) Serial.println("Checking MQTT connection... ");

    while (!mqtt.connected() && i < 10) {
        if (DEBUG) Serial.println("Attempting MQTT connection... " + espName);

        // Attempt to connect
        if (mqtt.connect(mqttServer, mqttPort)) {
            if (DEBUG) Serial.println("Connected");
        } else {
           Serial.print("MQTT connection failed! Error code = ");
           Serial.println(mqtt.connectError());

           // Wait 5 seconds before retrying
           delay(5000);
        }

        i++;
    }
}

void mqttSendMessage(String topic, float dato) {
    //MQTT Message
    char mqttTopicC[256];
    char playload[6];
    bool error = false;

    dtostrf(dato, 6, 2, playload);

    topic = mqttBaseTopic + "/" + topic;

    if (DEBUG) Serial.println("Publishing MQTT data... ");

    if (0 == mqtt.beginMessage(topic))
       error = true;

    mqtt.print(playload);

    if (0 == mqtt.endMessage())
        error = true;

    if (error) {
        Serial.print("MQTT Error publishing message.");
    } else if (DEBUG) {
        Serial.print("Publish - Topic:");
        Serial.print(mqttTopicC);
        Serial.print(" -> message: ");
        Serial.println(playload);
    }
}
