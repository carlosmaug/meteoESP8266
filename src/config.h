// -----------------------------------
// ------ PIN set configuration ------
// -----------------------------------
#define DHTTYPE DHT22
#define DHTPIN    4
#define MQPIN     5
#define SLCPIN   13
#define SDAPIN   14
#define MLPIN    16
#define MQDELAY 15000
#define MLDELAY   400

// Analog PIN this is the only possible value for ESP8266
int ANALOGPIN = A0; 

// --------------------
// ---- Debug mode ----
//---------------------
// if true print readings to console
#define DEBUG  true

//-----------------------------------------------------------------------
//--------------- GENARAL CONFIGURATION PARAMETERS ----------------------
//-----------------------------------------------------------------------

// Wifi AP 
// This AP is strarted if we cant connect to the WIFI
char apName[12] = "APname";
char apPass[13] = "password";

//NTP
char ntpServer[100] = "192.168.1.1";

// Interval between ntp updates (in milliseconds)
uint16_t ntpint = 600; 
const long ntpInterval = 3600000;  // 1h

// Sensor read interval (in milliseconds)
const long readInterval = 60000;   // 60s

//MQTT
const char* mqttServer = "192.168.1.3";
int mqttPort = 1883;
long lastMsg = 0;
char msg[50];
int value = 0;

//Other MQTT things
String espName = String("Meteo_1");
String mqttBaseTopic = String("home");

//OTA
//const char* host = espName;
const char* updatePath = "/firmware";
const char* updateUsername = "admin";
const char* updatePassword = "admin";
