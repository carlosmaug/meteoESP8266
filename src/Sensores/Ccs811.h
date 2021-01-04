#ifndef CCS811_H
#define CCS811_H
/*
   Bmp180.h - Class to access Bmp180 sensor
*/
#include <vector>
#include <Wire.h>
#include "../vendor/CCS811/src/ccs811.h"
#include "Ccs811Config.h"
#include "Sensores.h"

class Ccs811 : public Sensor {
public:
    float index, watt, indexMin = -1, indexMax, wattMin = -1, wattMax;

    Ccs811(std::vector <sensor> &sensors); 

    /*
     * Reads the sensor
     */
    void read(std::vector <sensor> &sensors); 

private:
    CCS811 *_ccs;
    String _idCo2;
    String _idVoc;

protected:
    void _setSensorInfo();
};


Ccs811::Ccs811(std::vector <sensor> &sensors) {
    int  count = 0;
    bool error = false;

    this->_ccs       = new CCS811(NWAKE, CCS811_ADDR);

    this->_ccs->set_i2cdelay(50);

    while (count < 20 && !this->_css->begin()) {
        count++;
	delay(500);
    }

    if (count >= 20) {
        Serial.println("Failed to init CCS811 sensor!");
	error = true;
    } else {
        count = 0;

        while (count < 20 && !this->_css->start(CCS811_MODE_1SEC) {
            count++;
            delay(500);
        }

        if (count >= 20) {
            Serial.println("Failed to start CCS811 sensor!");
            error = true;
        }
    }

    this->_numSensors = 2;
    this->_idCo2      = "CO2"+(String) random(9999);
    this->_idVoc      = "VOC"+(String) random(9999);

    this->_setSensorInfo();
    this->_addSensorInfo(sensors);
}


void Ccs811::read(std::vector <sensor> &sensors) {
    uint16_t co2, voc, code;
    bool error = false;
    
    this->_css->read(&co2, &voc, &code);

    if ( code == CCS811_ERRSTAT_OK ) { 
        this->_sensors[0].data = co2; // CO2 ppm
        this->_sensors[1].data = voc; // VOC ppb
    } else if ( errstat == CCS811_ERRSTAT_OK_NODATA ) {
	error = true;
    } else if ( errstat & CCS811_ERRSTAT_I2CFAIL ) { 
        Serial.println("CCS811: I2C error while reading data.");
	error = true;
    } else {
        Serial.print("CCS811: ERROR: "); 
	Serial.print(errstat, HEX); 
        Serial.print(" -> "); 
	Serial.println(this->_ccs->errstat_str(errstat));
        error = true;
    }

    if ( error ) {
        this->_sensors[0].data = 0;
        this->_sensors[1].data = 0;
    }    
    
    this->_setData(0, sensors);
    this->_setData(1, sensors);
}


void Veml6070::_setSensorInfo() {
    sensor sensor;

    sensor.id   = this->_idIdx;
    sensor.name = "CO2";
    sensor.unit = "ppm";
    sensor.img  = "co2.svg";
    this->_sensors.push_back(sensor);

    sensor.id   = this->_idRad;
    sensor.name = "VOC";
    sensor.unit = "ppb";
    sensor.img  = "voc.svg";
    this->_sensors.push_back(sensor);
}
#endif
