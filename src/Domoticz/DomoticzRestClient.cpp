/**
 *  DomoticzRestClient.cpp - Class to publish data into Domoticz
 */
#include "DomoticzRestClient.h"
#include <rBase64.h>

/**
 * Constructor
 */
DomoticzRestClient::DomoticzRestClient() {
    if (DEBUG) {
        Serial.print("Connecting to: ");
        Serial.print(this->restServer.c_str());
        Serial.print(" -> port: ");
        Serial.println(this->restPort);
    }

    if ( 0 == this->restSSL ) {
        this->_client = new RestClient(this->restServer.c_str(), this->restPort);
    } else {
        this->_client = new RestClient(this->restServer.c_str(), this->restPort);
	this->_client->setSsl(true);

#ifdef REST_ROOT_CERT
	static const char trustRoot[] PROGMEM = REST_ROOT_CERT;

	if (DEBUG) Serial.println("SSL cert:");
	if (DEBUG) Serial.println(trustRoot);

	this->_client->setTrustRoots(trustRoot);
#endif	
    }
}

int DomoticzRestClient::get(const char *path) {
    if (this->restAuth.isEmpty())
        this->_client->setHeader("Authorization: Basic " + rbase64.encode(this->restAuth));

    int statusCode = this->_client->get(path, &this->response);

    if (DEBUG) {
        Serial.print("DomoticzRestClient::Get: ");
        Serial.print(path);
        Serial.print(" -> Status code from server: ");
        Serial.println(statusCode);
    }

    return statusCode;
}

/**
 * Post data to rest server
 */
int DomoticzRestClient::post(const char *path) {
    if (this->restAuth.isEmpty())
        this->_client->setHeader("Authorization: Basic " + rbase64.encode(this->restAuth));

    int statusCode = this->_client->post(path, "", &this->response);

    if (DEBUG) {
        Serial.print("DomoticzRestClient::Post: ");
        Serial.print(path);
        Serial.print(" -> Status code from server: ");
        Serial.println(statusCode);
        Serial.print(" -> Response: ");
        Serial.println(statusCode);
    }

    return statusCode;
}

/**
 * Sends sensor data to Domoticz
 *
 * @param int 
 */
int DomoticzRestClient::sendData(String name, float data) {
    String path = REST_PATH;
    char *idx = "IDX";
    bool uv = false;

    if (DEBUG) Serial.println("Name " + name);

    if (-1 != name.indexOf("Temperatura DHT")) {
        path.replace(idx, REST_TEMP_IDX);
    } else if (-1 != name.indexOf("Humedad")) {
        path.replace(idx, REST_HUM_IDX);
    } else if (-1 != name.indexOf("Presion")) {
        path.replace(idx, REST_PRES_IDX);
    } else if (-1 != name.indexOf("Luz")) {
        path.replace(idx, REST_LUX_IDX);
    } else if (-1 != name.indexOf("Irradiancia Solar")) {
        path.replace(idx, REST_SOLRAD_IDX);
    } else if (-1 != name.indexOf("Indice UV")) {
        path.replace(idx, REST_UVIND_IDX);
	uv = true;
    } else if (-1 != name.indexOf("Radiacion UV")) {
        path.replace(idx, REST_UVRAD_IDX);
    } else {
	return 0;
    }

    char charData[8];

    dtostrf(data, 6, 2, charData);
    String stringData = String(charData);
    stringData.trim();

    if (uv) stringData = stringData + ";0"; 
    path.replace("VAL", stringData);

    if (DEBUG) Serial.println("Sending " + path);

    this->response = "";
    int code = this->post(path.c_str());

    if (DEBUG) Serial.println("Server Response: " + this->response);

    return code;
}
