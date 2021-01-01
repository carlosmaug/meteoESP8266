#ifndef WEB_H
#define WEB_H

/*
   Web.h - Class to create a web server
*/
#include <string>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>
#include "WebConfig.h"

class Web {
public:
    Web();

    void noEncontrada();
    void read();
    void reinicio();
    void setRaiz(String html);
    void setTitle(String title);
    String getRaiz();

private:
    ESP8266WebServer        *_server;
    ESP8266HTTPUpdateServer *_httpUpdater;

    String _pagInicio;

protected:
    String _header;
};

Web::Web() {
    this->setTitle(WEB_TITLE);

    this->_server      = new ESP8266WebServer(80);
    this->_httpUpdater = new ESP8266HTTPUpdateServer();

    this->_httpUpdater->setup(_server, UPDATE_PATH, UPDATE_USERNAME, UPDATE_PASSWORD);
    
    if (DEBUG) Serial.printf("HTTPUpdateServer ready! Open http://%s.local%s in your browser and login with username '%s' and password '%s'\n", espName.c_str(), UPDATE_PATH, UPDATE_USERNAME, UPDATE_PASSWORD);

    this->_server->on("/", [&](){this->_server->send(200, "text/html", this->_pagInicio);});
    this->_server->on("/reboot", [&](){this->reinicio();});
    this->_server->onNotFound([&](){this->noEncontrada();});
    this->_server->begin();
}	

void Web::read() {
    this->_server->handleClient();
}

void Web::setRaiz(String html) {
    this->_pagInicio = html; 
}
   
void Web::setTitle(String title) {
    this->_header = WEB_HEADER;
    this->_header.replace("{{TITLE}}", title);	
}	

String Web::getRaiz() {
    return this->_pagInicio;
}

void Web::noEncontrada() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += _server->uri();
    message += "\nMethod: ";
    message += (_server->method() == HTTP_GET)?"GET":"POST";
    message += "\nArguments: ";
    message += _server->args();
    message += "\n";
      
    for (uint8_t i = 0; i < _server->args(); i++) {
      message += " " + _server->argName(i) + ": " + _server->arg(i) + "\n";
    }
      
    _server->send(404, "text/plain", message);
}

void Web::reinicio() {
    String html = "<!DOCTYPE html>\
<html lang=es-ES>\
<head>\
<title>Reboot</title>\
<meta http-equiv=refresh content=60;url=/>\
<meta charset=UTF-8>\
</head>\
<body>\
<div style=text-align:center><p><b>Rebooting, please wait 60 seconds.<b></p>\
</body>\
</html>";

    _server->send(200, "text/html", html);
    delay(5000);
    ESP.restart();
}

#endif
