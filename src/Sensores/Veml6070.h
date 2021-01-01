#ifndef VEML6070_H
#define VEML6070_H
/*
   Bmp180.h - Class to access Bmp180 sensor
*/
#include <vector>
#include <Wire.h>
#include <Adafruit_VEML6070.h>
#include "Sensores.h"

class Veml6070 : public Sensor {
public:
    float index, watt, indexMin = -1, indexMax, wattMin = -1, wattMax;

    Veml6070(std::vector <sensor> &sensors); 

    /*
     * Reads the sensor
     */
    void read(std::vector <sensor> &sensors); 

private:
    Adafruit_VEML6070 *_veml;
    String _idIdx;
    String _idRad;

protected:
    void _setSensorInfo();
};


Veml6070::Veml6070(std::vector <sensor> &sensors) {
    this->_veml       = new Adafruit_VEML6070();
    
    this->_veml->begin(VEML6070_2_T);

    this->_numSensors = 2;
    this->_idIdx      = "IUV"+(String) random(9999);
    this->_idRad      = "RUV"+(String) random(9999);

    this->_setSensorInfo();
    this->_addSensorInfo(sensors);
}


void Veml6070::read(std::vector <sensor> &sensors) {
    int reading;

    reading     = _veml->readUV();
    this->_sensors[0].data = reading / 373.333; // Indice UV
    this->_sensors[1].data = reading / 50; // W/m2

    this->_setData(0, sensors);
    this->_setData(1, sensors);
}


void Veml6070::_setSensorInfo() {
    sensor sensor;

    sensor.id   = this->_idIdx;
    sensor.name = "Indice UV";
    sensor.unit = "IU";
    sensor.img  = "uv.svg";
    this->_sensors.push_back(sensor);

    sensor.id   = this->_idRad;
    sensor.name = "Radiacion UV";
    sensor.unit = "w/m2";
    sensor.img  = "uv.svg";
    this->_sensors.push_back(sensor);
}
#endif
