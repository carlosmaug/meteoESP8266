#ifndef WEB_H
#define WEB_H

#ifndef UPDATEPATH
#define UPDATEPATH "/firmware"
#endif

#ifndef UPDATEUSERNAME
#define UPDATEUSERNAME "admin"
#endif

#ifndef UPDATEPASSWORD
#define UPDATEPASSWORD "1Carlos."
#endif

/*
   Web.h - Class to create a web server
*/
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>

class Web {
public:
    Web();

    void noEncontrada();
    void read();
    void reinicio();
    void setRaiz(String html);
    String getRaiz();

private:
    ESP8266WebServer        *_server;
    ESP8266HTTPUpdateServer *_httpUpdater;

    String _pagInicio;
};

Web::Web() {
    _server      = new ESP8266WebServer(80);
    _httpUpdater = new ESP8266HTTPUpdateServer();

    _httpUpdater->setup(_server, UPDATEPATH, UPDATEUSERNAME, UPDATEPASSWORD);
    
    if (DEBUG) Serial.printf("HTTPUpdateServer ready! Open http://%s.local%s in your browser and login with username '%s' and password '%s'\n", espName.c_str(), UPDATEPATH, UPDATEUSERNAME, UPDATEPASSWORD);

    _server->on("/", [&](){_server->send(200, "text/html", _pagInicio);});
    _server->on("/r", [&](){reinicio();});
    _server->onNotFound([&](){noEncontrada();});
    _server->begin();
}	

void Web::read() {
    _server->handleClient();
}

void Web::setRaiz(String html) {
    _pagInicio = html; 
}

String Web::getRaiz() {
    return _pagInicio;
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
<meta http-equiv=refresh content=20;url=/>\
<meta charset=UTF-8>\
</head>\
<body>\
<div style=text-align:center><p><b>Rebooting, please wait.<b></p>\
</body>\
</html>";

    _server->send(200, "text/html", html);
    delay(10000);
    ESP.restart();
}

#endif
