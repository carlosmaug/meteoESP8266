#ifdef WIFIC_H
#ifndef DEBUG
#define DEBUG false
#endif
#ifndef MQTT_H
#define MQTT_H

/*
   Mqtt.h - Class to add mqtt support
*/
#include <WiFiClient.h>
#include <ArduinoMqttClient.h>

class Mqtt {
public:
    Mqtt();
    void callback();
    void publishValues();
    void reconnect();
    void setEspName(String espname);
    void sendMessage(String, float);

    /**
     * @brief This function is called when a Mqtt message arrives
     */
    static void calllback(char* topic, byte* payload, unsigned int length) {
        if ( DEBUG ) {
            Serial.print("Message arrived [");
            Serial.print(topic);
            Serial.print("] ");
    
            for (int i = 0; i < length; i++) {
                Serial.print((char)payload[i]);
            }
    
            Serial.println();
        }
    }

private:
    MqttClient * _mqtt;
    String espName;
    char   espNameC[50];
    String mqttBaseTopic;
};

/**
 * Constructor
 */

//Mqtt::Mqtt(Client& client) {
Mqtt::Mqtt() {
    WiFiClient client;

this->espName       = String("Meteo_1");
this->mqttBaseTopic = String("casa");

    this->_mqtt = new MqttClient(client);
   
    this->reconnect(); 
    this->setEspName("Meteo_1");
   //mqtt->setCallback(Mqtt::callback);
    
    this->mqttBaseTopic = this->mqttBaseTopic + "/" + this->espName;
}

/**
 * @brief Sets the esp name
 */
void Mqtt::setEspName(String espname) {
    this->espName = espname;	
    espname.toCharArray(this->espNameC, espname.length() + 1);
}    

/**
 * @brief reconnets to mqtt server
 */
void Mqtt::reconnect() {
    // Loop until we're reconnected
    int i = 0; 
const char mqttServer[] = "10.0.1.3";
int mqttPort = 1883;


    if (DEBUG) Serial.println("Checking MQTT connection... ");

    while (!this->_mqtt->connected() && i < 10) {
        if (DEBUG) Serial.println("Attempting MQTT connection... " + espName);
        
	// Attempt to connect
        if (this->_mqtt->connect(mqttServer, mqttPort)) {
            if (DEBUG) Serial.println("Connected");
        } else {
           Serial.print("MQTT connection failed! Error code = ");
           Serial.println(this->_mqtt->connectError());

           // Wait 5 seconds before retrying
           delay(5000);
        }

	i++;
    }
}

/**
 * @brief sends Mqtt message to server
 */
void Mqtt::sendMessage(String topic, float dato) {
    //MQTT Message
    char mqttTopicC[256];
    char playload[6];
    bool error = false;

    dtostrf(dato, 6, 2, playload);

    topic = this->mqttBaseTopic + "/" + topic;
    topic.toCharArray(mqttTopicC, topic.length() + 1);
   
    if (DEBUG) Serial.println("Publishing MQTT data... ");
   
    if (0 == this->_mqtt->beginMessage(topic))
       error = true;

    this->_mqtt->print(mqttTopicC);
    this->_mqtt->print(playload);
    
    if (0 == this->_mqtt->endMessage()) 
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

#endif
#endif
