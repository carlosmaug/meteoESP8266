#ifndef DEBUG
#define DEBUG false
#endif

#ifndef REST_H
#define REST_H
#define HTTP_DEBUG true

#include "../Net/RestClient.h"
#include "RestConfig.h"

/**
 *  DomoticzRestClient.cpp - Class to publish data into Domoticz
 */
class DomoticzRestClient {

public:
    DomoticzRestClient();
    int  get(const char *path);
    int  post(const char *path);
    int  sendData(String name, float data);

    String restServer      = REST_SERVER;
    int    restPort        = REST_PORT;
    bool   restSSL         = REST_SSL;
    String restAuth        = REST_AUTH;
    String response;

private:
    RestClient *_client;
    char *data;
};
#endif
