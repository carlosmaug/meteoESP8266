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
	virtual void read(vector<sensor> &sensors) = 0;

	/*
	 * Retur the number os sensors implemented by the sensor.
	 * For example DHT22 sensor has 2 sensors preassure and humidity
	 */
	int getNumSensors();

protected:
	int            _numSensors;
	vector<sensor> _sensors;

	/*
	 * Sets sensor information into sensors struct
	 */
	virtual void _setSensorInfo() = 0;

	/*
	 * Adds sensor info to the argument struct
	 */
	void _addSensorInfo(vector<sensor> &sensors);


	/*
	 * Sets minimun and maximun values readed by a sensor
	 */
	void _setData(int idx, vector<sensor> &sensors);
};

void Sensor::_addSensorInfo(vector<sensor> &sensors) {
	for (int i = 0; i < this->_sensors.size(); i++) {
		sensors.push_back(this->_sensors[i]);
	}	
}	

int Sensor::getNumSensors() {
	return this->_numSensors;
}


void Sensor::_setData(int pos, vector<sensor> &sensors) {
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
