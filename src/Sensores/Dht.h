#ifndef DHT22_H
#define DHT22_H
/*
   DHT22.h - Class to access DHT22 sensor
*/
#include <DHT.h>
#include "DhtConfig.h"
#include "Sensores.h"

// Initialize DHT sensor 
// NOTE: For working with a faster than ATmega328p 16 MHz Arduino chip, like an ESP8266,
// you need to increase the threshold for cycle counts considered a 1 or 0.
// You can do this by passing a 3rd parameter for this threshold.  It's a bit
// of fiddling to find the right value, but in general the faster the CPU the
// higher the value.  The default for a 16mhz AVR is a value of 6.  For an
// Arduino Due that runs at 84mhz a value of 30 works.
// This is for the ESP8266 processor on ESP-01 

class Dht22 : public Sensor {
public:
    Dht22(vector<sensor> &sensors); 

    /*
     * Reads the sensor
     */
    void read(vector<sensor> &sensors);

private:
    DHT *_dht;
    String _idTemp;
    String _idHum;

protected:    
    void _setSensorInfo();
};

Dht22::Dht22(vector<sensor> &sensors) {
    _dht = new DHT(DHTPIN, DHTTYPE, 11);

    _dht->begin();

    this->_numSensors = 2;
    this->_idTemp     = "Temp"+(String) random(9999);
    this->_idHum      = "Hum"+(String) random(9999);

    this->_setSensorInfo();
    this->_addSensorInfo(sensors);
}

void Dht22::read(vector<sensor> &sensors) {
    int  count = 0;
    bool error = false;

    do {
        this->_sensors[0].data = this->_dht->readTemperature();
        this->_sensors[1].data = this->_dht->readHumidity();  
        count++;
    } while (count < 20 && (isnan(this->_sensors[0].data) || isnan(this->_sensors[1].data)));

    if (count >= 20) {
        Serial.println("Failed to read from DHT sensor!");
	error = true;
    }

    this->_setData(0, sensors);
    this->_setData(1, sensors);
} 

void Dht22::_setSensorInfo() {
    sensor sensor;

    sensor.id   = this->_idTemp;
    sensor.name = "Temperatura DHT";
    sensor.unit = "ºC";
    sensor.img  = "temp.svg";
    this->_sensors.push_back(sensor);

    sensor.id   = this->_idHum;
    sensor.name = "Humedad DHT";
    sensor.unit = "%";
    sensor.img  = "humedad.svg";
    this->_sensors.push_back(sensor);
}

#endif
