#ifndef BMP180_H
#define BMP180_H
/*
   Bmp180.h - Class to access Bmp180 sensor
*/
#include <vector>
#include <Wire.h>
#include <SFE_BMP180.h>
#include "Sensores.h"

class Bmp180 : public Sensor {
public:
    Bmp180(std::vector <sensor> &sensors);

    /*
     * Reads the sensor
     */
    void read(std::vector <sensor> &sensors);

private:
    SFE_BMP180 _bmp;
    String _idTemp;
    String _idPres;

protected:
    int  _start();
    void _setSensorInfo();
};


Bmp180::Bmp180(std::vector <sensor> &sensors) {
    this->_start();

    this->_idTemp     = "Temp"+(String) random(9999);
    this->_idPres     = "Pres"+(String) random(9999);
    this->_numSensors = 2;

    this->_setSensorInfo();
    this->_addSensorInfo(sensors);
}

/**
 * Initialize the sensor 
 *
 * It is important to get calibration values stored on the device.
 *
 * @return 0 if initialitation failed 1 if all was ok
 */
int Bmp180::_start() {
    int i = 0;

    while (i < 5 && 0 == _bmp.begin()) {
        i++;
	delay(500);
    }

    if (i < 5) {
        Serial.println("BMP180 init success");
    } else {
        // Oops, something went wrong, this is usually a connection problem
        Serial.println("BMP180 init fail");
    }
}

/**
 * Read sensor values
 */
void Bmp180::read(std::vector <sensor> &sensors) {
    int status;
    double temp, pres;

    // Retrieve the completed pressure measurement:
    // Note that the measurement is stored in the variable P.
    // Note also that the function requires the previous temperature measurement (T).
    // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
    // Function returns 1 if successful, 0 if failure.
    status = _bmp.startTemperature();

    if ( 0 == status ) {
	status = this->_start();

	if (0 != status)
	    status = _bmp.startTemperature();
    }

    if (0 != status) {
        // Wait for the measurement to complete:
        delay(status);
        status = _bmp.getTemperature(temp);
        this->_sensors[0].data = (float) temp;

        if (0 != status) {
            status = _bmp.startPressure(3);

            if (0 != status) {
                // Wait for the measurement to complete:
                delay(status);
                status = _bmp.getPressure(pres, temp);
        	this->_sensors[1].data = (float) pres;

                if (0 == status) {
                    Serial.println("BMP: error retrieving pressure measurement\n");
                }
            } else {
                Serial.println("BMP: error starting preasure measurement\n");
            }
	}  else {
             Serial.println("BMP: error reading temperature measurement\n");
        }
    } else {
        Serial.println("BMP: error starting temperature measurement\n");
    }
 
    this->_setData(0, sensors);
    this->_setData(1, sensors);
}

/**
 * Adds sensor info to sensors array
 */
void Bmp180::_setSensorInfo() {
    sensor sensor;

    sensor.id   = this->_idTemp;
    sensor.name = "Temperatura BMP";
    sensor.unit = "ºC";
    sensor.img  = "temp.svg";
    this->_sensors.push_back(sensor);

    sensor.id   = this->_idPres;
    sensor.name = "Presion BMP";
    sensor.unit = "hPa";
    sensor.img  = "presion.svg";
    this->_sensors.push_back(sensor);
}

#endif
