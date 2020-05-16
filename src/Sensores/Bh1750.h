#ifndef BH1750_H
#define BH1750_H
/*
 * Bmp180.h - Class to access Bmp180 sensor
 */
#include <Wire.h>
#include <BH1750.h>
#include "Sensores.h"

class Bh1750 : public Sensor {
public:
    Bh1750(vector<sensor> &sensors); 

    /*
     * Reads the sensor
     */
    void read(vector<sensor> &sensors);

private:
    BH1750 *_bh;
    String _idLuz;
    String _idIrradiancia;

protected:
    void _setSensorInfo();
};

Bh1750::Bh1750(vector<sensor> &sensors) {
    this->_bh = new BH1750();

    this->_bh->begin();

    this->_numSensors    = 2;
    this->_idLuz         = "Luz"+(String) random(9999);
    this->_idIrradiancia = "Irradiancia"+(String) random(9999);

    this->_setSensorInfo();
    this->_addSensorInfo(sensors);
}

/*
 * Reads the sensor
 */
void Bh1750::read(vector<sensor> &sensors) {
    this->_sensors[0].data = this->_bh->readLightLevel();
    this->_sensors[1].data = this->_sensors[0].data/683.0; //Lux to Watts/m2

    this->_setData(0, sensors);
    this->_setData(1, sensors);
}

void Bh1750::_setSensorInfo() {
    sensor sensor;

    sensor.id   = this->_idLuz;
    sensor.name = "Luz";
    sensor.unit = "lux";
    sensor.img  = "solar.svg";
    this->_sensors.push_back(sensor);

    sensor.id   = this->_idIrradiancia;
    sensor.name = "Irradiancia Solar";
    sensor.unit = "w/m2";
    sensor.img  = "solar.svg";
    this->_sensors.push_back(sensor);
}

#endif
