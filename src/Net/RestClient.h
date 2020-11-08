#ifndef RestClient_h
#define RestClient_h

#define HTTP_DEBUG

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

class RestClient {

public:
    RestClient(const char* host);
    RestClient(const char* _host, int _port);

    // Set Content-Type Header
    void setContentType(const char*);
    // Set a Request Header
    void setHeader(const char*);
    // Set ssl on/off
    void setSsl(bool);
    // Addmit self signed certificates? on/off
    void setSelfsigned(bool selfSigned);
    // Set certificate firgerprint
    void setFingerprint(const char* fingerPrint);
    // Set trusted CA root certs
    void setTrustRoots(const char* trustRoots);
    // Set certificate key
    void setCertKey(const char* certKey);

    // Create connection client 
    void init();

    // Generic HTTP Request
    int request(const char* method, const char* path, const char* body, String* response);
    // GET path
    int get(const char*);
    // GET path and response
    int get(const char*, String*);

    // POST path and body
    int post(const char* path, const char* body);
    // POST path and body and response
    int post(const char* path, const char* body, String*);

    // PATCH path and body
    int patch(const char* path, const char* body);
    // PATCH path and body and response
    int patch(const char* path, const char* body, String*);

    // PUT path and body
    int put(const char* path, const char* body);
    // PUT path and body and response
    int put(const char* path, const char* body, String*);

    // DELETE path
    int del(const char*);
    // DELETE path and body
    int del(const char*, const char*);
    // DELETE path and response
    int del(const char*, String*);
    // DELETE path and body and response
    int del(const char*, const char*, String*);

private:
    WiFiClient *client;
    BearSSL::WiFiClientSecure *sslClient;

    int readResponse(String*);
    void write(const char*);

    bool selfSigned = false;
    bool ssl        = false;
    int port;
    int num_headers = 0;
    const char* host;
    const char* headers[10];
    const char* contentType = "application/x-www-form-urlencoded";
    const char* certKey     = NULL;
    const char* fingerPrint = NULL;
    const char* trustRoots  = NULL;
};

#endif
