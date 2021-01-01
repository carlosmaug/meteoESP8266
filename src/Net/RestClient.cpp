#include "RestClient.h"

#ifdef HTTP_DEBUG
#define HTTP_DEBUG_PRINT(string) (Serial.print(string))
#endif

#ifndef HTTP_DEBUG
#define HTTP_DEBUG_PRINT(string)
#endif

RestClient::RestClient(const char* _host) {
    this->host = _host;
    this->port = 80;
}

RestClient::RestClient(const char* _host, int _port) {
    this->host = _host;
    this->port = _port;
}

void RestClient::setContentType(const char* contentTypeValue){
    this->contentType = contentTypeValue;
}

void RestClient::setHeader(const char* header){
    this->headers[this->num_headers] = header;
    this->num_headers++;
}

void RestClient::setSsl(bool ssl = true) {
    this->ssl = ssl;
}

void RestClient::setSelfsigned(bool selfSigned = true) {
    this->selfSigned = selfSigned;
}

void RestClient::setFingerprint(const char* fingerPrint) {
    this->fingerPrint = fingerPrint;
}

void RestClient::setTrustRoots(const char* trustRoots) {
    this->trustRoots = trustRoots;
}

void RestClient::setCertKey(const char* certKey) {
    this->certKey = certKey;
}

/**
 * Init ssl client
 *
 * If selfSigned is true then we accept self signed certs
 * Ssl certs can be validated depending on the data provided, if no 
 * validation data is provided we accept insecure connections.
 */
void RestClient::init() {
    if (this->ssl) {
        this->sslClient = new BearSSL::WiFiClientSecure();

        if (this->selfSigned)
            this->sslClient->allowSelfSignedCerts();

        if (NULL != this->fingerPrint) {
	    HTTP_DEBUG_PRINT("HTTPS fingetprint validaton\n");
            this->sslClient->setFingerprint(this->fingerPrint);
        } else if (NULL != this->trustRoots) {
	    HTTP_DEBUG_PRINT("HTTPS trunsted root validaton\n");
	    BearSSL::X509List certs(this->trustRoots);
            this->sslClient->setTrustAnchors(&certs);
        } else if (NULL != this->certKey) {
	    HTTP_DEBUG_PRINT("HTTPS public key validaton\n");
            BearSSL::PublicKey key(this->certKey);
            this->sslClient->setKnownKey(&key);
        } else {
	    HTTP_DEBUG_PRINT("HTTPS insecure connection\n");
            this->sslClient->setInsecure();
        }
    } else {
	HTTP_DEBUG_PRINT("HTTP insecure connection\n");
	this->client = new WiFiClient();
    }
}	

// GET path
int RestClient::get(const char* path) {
    return request("GET", path, NULL, NULL);
}

//GET path with response
int RestClient::get(const char* path, String* response) {
    return request("GET", path, NULL, response);
}

// POST path and body
int RestClient::post(const char* path, const char* body) {
    return request("POST", path, body, NULL);
}

// POST path and body with response
int RestClient::post(const char* path, const char* body, String* response) {
    return request("POST", path, body, response);
}

// PATCH path and body
int RestClient::patch(const char* path, const char* body) {
    return request("PATCH", path, body, NULL);
}

// PATCH path and body with response
int RestClient::patch(const char* path, const char* body, String* response) {
    return request("PATCH", path, body, response);
}

// PUT path and body
int RestClient::put(const char* path, const char* body) {
    return request("PUT", path, body, NULL);
}

// PUT path and body with response
int RestClient::put(const char* path, const char* body, String* response) {
    return request("PUT", path, body, response);
}

/**
 * Perform HTTP Delete request
 *
 * @param path     URL to retreive from host
 *
 * @relatesalso request
 */
int RestClient::del(const char* path) {
    return request("DELETE", path, NULL, NULL);
}

/**
 * Perform HTTP Delete request
 *
 * @param path     URL to retreive from host
 * @param response Server response to request
 *
 * @relatesalso request
 */
int RestClient::del(const char* path, String* response) {
    return request("DELETE", path, NULL, response);
}

/**
 * Perform HTTP Delete request
 *
 * @param path     URL to retreive from host
 * @param body     Data to send to server within the request
 *
 * @relatesalso request
 */
int RestClient::del(const char* path, const char* body ) {
    return request("DELETE", path, body, NULL);
}

/**
 * Perform HTTP Delete request
 *
 * @param path     URL to retreive from host
 * @param body     Data to send to server within the request
 * @param response Server response to request
 *
 * @relatesalso request
 */
int RestClient::del(const char* path, const char* body, String* response) {
    return request("DELETE", path, body, response);
}

/**
 * Send data over an stablished connection
 *
 * @param string data to send
 */
void RestClient::write(const char* string){
    if(this->ssl) {
        HTTP_DEBUG_PRINT("\nSSL Print: ");
        HTTP_DEBUG_PRINT(string);
        this->sslClient->print(string);
    } else {
        HTTP_DEBUG_PRINT("\nHTTP Print: ");
        HTTP_DEBUG_PRINT(string);
        client->print(string);
    }
}

/**
 * Connect to host and perform a request.
 * 
 * @param method   HTTP method to use (GET, PUT, POST, DELETE, etc)
 * @param path     URL to retreive from host
 * @param body     Data to send to server within the request
 * @param response Server response to request
 *
 * @return int 0 if failed 1 otherwise
 */
int RestClient::request(const char* method, const char* path, const char* body, String* response) {
    int statusCode = 0;
    int i = 0;

    if (this->ssl) {
	HTTP_DEBUG_PRINT("HTTPS connecting to: ");
	HTTP_DEBUG_PRINT(host);
	HTTP_DEBUG_PRINT(" ");
	HTTP_DEBUG_PRINT(port);
	HTTP_DEBUG_PRINT("\n");

	this->init();

	do {
	    this->sslClient->connect(host, port);
            statusCode = this->sslClient->connected(); 
 
            if (0 == statusCode) {
                HTTP_DEBUG_PRINT("HTTPS Connection failed with code: ");
                HTTP_DEBUG_PRINT(sslClient->getLastSSLError());
                HTTP_DEBUG_PRINT("\n");
            }

	    i++;
	} while ( 0 == statusCode && i < 3);
    } else {
        HTTP_DEBUG_PRINT("HTTP: connect\n");
	this->init();

	this->client->connect(host, port);

        if (!this->client->connected()) {
            HTTP_DEBUG_PRINT("HTTP Connection failed\n");
            return 0;
        }
    }

    HTTP_DEBUG_PRINT("HTTP: connected\n");
    HTTP_DEBUG_PRINT("REQUEST: \n");

    String request = String(method) + " " + String(path) + " HTTP/1.1\r\n";

    for (int i = 0; i < this->num_headers; i++) {
        request += String(headers[i]) + "\r\n";
    }

    request += "Host: " + String(host) +  ":" + String(port) + "\r\n";
    request += "Connection: close\r\n";

    if (body != NULL) {
        char contentLength[30];
        sprintf(contentLength, "Content-Length: %d\r\n", strlen(body));
        request += String(contentLength);
        request += "Content-Type: " + String(contentType) + "\r\n";
    }

    request += "\r\n";

    if (body != NULL) {
        request += String(body);
        request += "\r\n\r\n";
    }

    write(request.c_str());

    HTTP_DEBUG_PRINT("\nEND REQUEST\n");

    // Cleanup
    this->num_headers = 0;
    
    HTTP_DEBUG_PRINT("HTTP: call readResponse\n");

    if (this->ssl) {
    	statusCode = this->readResponse(response, this->sslClient);
    } else {
    	statusCode = this->readResponse(response, this->client);
    }

    this->stop();

    delay(50);
    HTTP_DEBUG_PRINT("HTTP: client stopped\n");

    return statusCode;
}

/**
 * Read response from server after sending a request
 *
 * @param response pointer where response is archived
 *
 * @return int HTTP response code
 */
int RestClient::readResponse(String* response, WiFiClient *cli) {
    // An HTTP request ends with a blank line
    bool currentLineIsBlank = true;
    bool httpBody           = false;
    bool inStatus           = false;
    int  i                  = 0;
    int  code               = 0;
    char statusCode[4];
    void* http_client;

    HTTP_DEBUG_PRINT("HTTP: RESPONSE: \n");

    while (cli->connected()) {
        HTTP_DEBUG_PRINT(".");

        if (cli->available()) {
            HTTP_DEBUG_PRINT(",");

            char c = cli->read();
            HTTP_DEBUG_PRINT(c);

            if (c == ' ' && !inStatus) 
                inStatus = true;

            if (inStatus && i < 3 && c != ' ') {
                statusCode[i] = c;
                i++;
            }

            if (i == 3) {
                statusCode[i] = '\0';
                code = atoi(statusCode);
            }

            if (httpBody) {
                // Only write response if its not null
                if(response != NULL) 
    		    response->concat(c);
            } else {
                if (c == '\n' && currentLineIsBlank) 
                    httpBody = true;

                if (c == '\n') {
                    // You're starting a new line
                    currentLineIsBlank = true;
                } else if (c != '\r') { // You've gotten a character on the current line
                    currentLineIsBlank = false;
                }
            }
        }
    }

    return code;
}

/**
 * Closes connection to server
 */
void RestClient::stop() {
    HTTP_DEBUG_PRINT("HTTP: Close connection\n");

    if (this->ssl) {
        this->sslClient->stop();
    } else {
        this->client->stop();
    }
}
