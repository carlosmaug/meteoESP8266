#ifndef DEBUG
#define DEBUG false
#endif
#ifndef CONF_H
#define CONF_H
/*
   Config.h - Class to retreive and store configurations
*/

#include "Fs.h"
#include <ArduinoJson.h>

class Config {
protected:    
    Files* f;
    struct Cnf {
	bool debug;
	long readInterval;
        String name;
        struct Mqtt {
	    String server;
	    int port;
	    String baseTopic;
	} mqtt;
	struct Ntp {
	    String server;
	    int port;
            int interval;
	} ntp;
	struct Wifi {
	    String apName;
	    String password;
	} wifi;
    } config;

public:
    Config();
    Cnf  Read();
    bool Write(Cnf content);

};

Config::Config() {
    const size_t size = JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(6) + 270;
    DynamicJsonDocument conf(size);
    const char* json;

    this->f          = new Files();
    String config    = this->f->Read();
    config.copy(json, config.size() + 1);
    json[config.size()] = "\0";

    deserializeJson(config, json);

    this->config.debug        = conf["debug"]        | 0;
    this->config.readInterval = conf["readInterval"] | 60;
    this->config.name         = conf["name"]         | "esp8266";

    JsonObject mqtt = conf["mqtt"];
    this->config.mqtt.server    = mqtt["server"]    | ""; 
    this->config.mqtt.port      = mqtt["port"]      | 1183;
    this->config.mqtt.baseTopic = mqtt["baseTopic"] | this->config.name;

    JsonObject ntp = conf["ntp"];
    this->config.ntp.server   = ntp["server"]   | "";
    this->config.ntp.port     = ntp["port"]     | 123;
    this->config.ntp.interval = ntp["interval"] | 600; 

    JsonObject wifi = conf["wifi"];
    this->config.wifi.apName   = wifi["apName"]   | "";
    this->config.wifi.password = wifi["password"] | "";

 }

Cnf Config::Read() {
   return this->config;
}

bool Config::Write(Cnf conf) {
    const size_t size = JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(6) + 270;
    DynamicJsonDocument conf(size);
    String json;

    conf["debug"]        = this->config.debug;
    conf["readInterval"] = this->config.readInterval;
    conf["name"]         = this->config.name;

    JsonObject mqtt = conf.createNestedObject("mqtt");
    mqtt["server"]    = this->config.mqtt.server;
    mqtt["port"]      = this->config.mqtt.port;
    mqtt["baseTopic"] = this->config.mqtt.baseTopic;

    JsonObject ntp = conf.createNestedObject("ntp");
    ntp["server"]   = this->config.ntp.server;
    ntp["port"]     = this->config.ntp.port;
    ntp["interval"] = this->config.ntp.interval;

    JsonObject wifi = conf.createNestedObject("wifi");
    wifi["apName"]   = this->config.wifi.apName;
    wifi["password"] = this->config.wifi.password;

    serializeJson(conf, json);
    return this-f->Write(json);
}

#endif
