#ifndef DEBUG
#define DEBUG false
#endif

#ifndef REST_H
#define REST_H
#define HTTP_DEBUG true

/*
   Rest.h - Class to add rest client support
*/
#include <RestClient.h>
#include "RestConfig.h"
#include <rBase64.h>

class EspRestClient {
public:
    EspRestClient();
    int get(const char *path);
    int post(const char *path);
    int sendData(String name, float data);
    String restServer      = REST_SERVER;
    int    restPort        = REST_PORT;
    bool   restSSL         = REST_SSL;
    String restFingerPrint = REST_FINGERPRINT;
    String restAuth        = REST_AUTH;
    String response;

private:
    RestClient *_client;
    char *data;
};

/**
 * Constructor
 */

EspRestClient::EspRestClient() {
    if (DEBUG) {
        Serial.print("Connecting to: ");
        Serial.print(this->restServer.c_str());
        Serial.print(" -> port: ");
        Serial.println(this->restPort);
    }

    if ( 0 == this->restSSL ) {
        this->_client = new RestClient(this->restServer.c_str(), this->restPort);
    } else {
	static const char trustRoot[] PROGMEM = REST_ROOT_CERT;

	if (DEBUG) Serial.println("SSL cert:");
	if (DEBUG) Serial.println(trustRoot);

        this->_client = new RestClient(this->restServer.c_str(), this->restPort);
	this->_client->setSsl(true);
	this->_client->setTrustRoots(trustRoot);
	this->_client->init();
    }
}

int EspRestClient::get(const char *path) {
    if (this->restAuth.isEmpty())
        this->_client->setHeader("Authorization: Basic " + rbase64.encode(this->restAuth));

    int statusCode = this->_client->get(path, &this->response);

    if (DEBUG) {
        Serial.print("EspRestClient::Get: ");
        Serial.print(path);
        Serial.print(" -> Status code from server: ");
        Serial.println(statusCode);
    }

    return statusCode;
}

/**
 * Post data to rest server
 */
int EspRestClient::post(const char *path) {
    if (this->restAuth.isEmpty())
        this->_client->setHeader("Authorization: Basic " + rbase64.encode(this->restAuth));

    int statusCode = this->_client->post(path, "", &this->response);

    if (DEBUG) {
        Serial.print("EspRestClient::Post: ");
        Serial.print(path);
        Serial.print(" -> Status code from server: ");
        Serial.println(statusCode);
        Serial.print(" -> Response: ");
        Serial.println(statusCode);
    }

    return statusCode;
}

int EspRestClient::sendData(String name, float data) {
    String path = REST_PATH;
    char *idx = "IDX";

    if (DEBUG) Serial.println("Name " + name);

    if (-1 != name.indexOf("Temperatura DHT")) {
        path.replace(idx, REST_TEMP_IDX);
    } else if (-1 != name.indexOf("Humedad")) {
        path.replace(idx, REST_HUM_IDX);
    } else if (-1 != name.indexOf("Presion")) {
        path.replace(idx, REST_PRES_IDX);
    } else if (-1 != name.indexOf("Lux")) {
        path.replace(idx, REST_LUX_IDX);
    } else if (-1 != name.indexOf("Irradiacia")) {
        path.replace(idx, REST_SOLRAD_IDX);
    } else if (-1 != name.indexOf("Indice")) {
        path.replace(idx, REST_UVIND_IDX);
    } else if (-1 != name.indexOf("Rediacion")) {
        path.replace(idx, REST_UVRAD_IDX);
    } else {
	return 0;
    }

    char charData[8];
    dtostrf(data, 6, 2, charData);
    String stringData = String(charData);
    stringData.trim();
    path.replace("VAL", stringData);

    if (DEBUG) Serial.println("Sending " + path);

    this->response = "";
    int code = this->post(path.c_str());

    if (DEBUG) Serial.println("Server Response: " + this->response);

    return code;
}

#endif
