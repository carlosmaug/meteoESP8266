#ifndef SENSORES_H
#define SENSORES_H
#include "config.h"
#include <vector>

/*
   Sensores.h - Abstract class that shoud be inherit by all sensor classes
*/

class Sensor {
public:
	/*
	 * Reads the sensor
	 */
	virtual void read(std::vector <sensor> &sensors) = 0;

	/*
	 * Retur the number os sensors implemented by the sensor.
	 * For example DHT22 sensor has 2 sensors preassure and humidity
	 */
	int getNumSensors();

protected:
	int            _numSensors;
	std::vector <sensor> _sensors;

	/*
	 * Sets sensor information into sensors struct
	 */
	virtual void _setSensorInfo() = 0;

	/*
	 * Adds sensor info to the argument struct
	 */
	void _addSensorInfo(std::vector <sensor> &sensors);


	/*
	 * Sets minimun and maximun values readed by a sensor
	 */
	void _setData(int idx, std::vector <sensor> &sensors);
};

/**
 * Adds sensor information to the sensonrs array
 */
void Sensor::_addSensorInfo(std::vector <sensor> &sensors) {
	for (int i = 0; i < this->_sensors.size(); i++) {
		sensors.push_back(this->_sensors[i]);
	}	
}	

/**
 * Retuns the number of sensors we have
 */
int Sensor::getNumSensors() {
	return this->_numSensors;
}

/**
 * Stores de actual values, maximun and minimum values of the sensonrs
 *
 * @param pos     int    Position of thesensonr into de sensors array
 * @param sensors vector Sensors data 
 */
void Sensor::_setData(int pos, std::vector <sensor> &sensors) {
	if (this->_sensors[pos].min > this->_sensors[pos].data || -1 == this->_sensors[pos].min) this->_sensors[pos].min = this->_sensors[pos].data;
	if (this->_sensors[pos].max < this->_sensors[pos].data) this->_sensors[pos].max = this->_sensors[pos].data;

	if (-1 == this->_sensors[pos].idx) {
		for (int i = 0; i < sensors.size(); i++) {
			if (sensors[i].id == this->_sensors[pos].id) { 
				this->_sensors[pos].idx = i;
			}	
		}
    	}

	sensors[this->_sensors[pos].idx].data = this->_sensors[pos].data;
	sensors[this->_sensors[pos].idx].min  = this->_sensors[pos].min;
	sensors[this->_sensors[pos].idx].max  = this->_sensors[pos].max;
}    
#endif
