// -----------------------------------
// ------ PIN set configuration ------
// -----------------------------------
#define DHTTYPE DHT22
// REAL PINS
#define DHTPIN D4
#define SLCPIN D1
#define SDAPIN D2

#define MLDELAY   400

// Analog PIN this is the only possible value for ESP8266
int ANALOGPIN = A0;

// --------------------
// ---- Debug mode ----
//---------------------
// if true print readings to console
#define DEBUG true


//-----------------------------------------------------------------------
//--------------- GENARAL CONFIGURATION PARAMETERS ----------------------
//-----------------------------------------------------------------------
#define ESPNAME "Meteo_1"

// Wifi AP 
// This AP is strarted if we cant connect to the WIFI
char apName[12] = "espConfigPA";
char apPass[13] = "1Carlos.casa";

//NTP
char ntpServer[100] = "hora.roa.es";

uint16_t ntpint = 600; 

// Interval between ntp updates (in seconds)
const long ntpInterval = 3600;  // 1h

// Sensor read interval (in seconds)
const long readInterval = 80;   // 60s

// Restarting interval to prevent system hangouts
const long resetInterval = 7200;   // 2h

//MQTT
#define MQTTSERVER  String("10.0.1.3")
#define MQTTPORT int(1883)


//Other MQTT things
String espName = String("Meteo_1");
String mqttBaseTopic = String("casa");


//MQTT
const char* mqttServer = "10.0.1.3";
int mqttPort = 1883;


//OTA
//const char* host = espName;
#define UPDATEPATH "/firmware"
#define UPDATEUSERNAME "admin"
#define UPDATEPASSWORD "admin"
