// if true print readings to console
#define DEBUG true

#define ESPNAME "Meteo_1"

// Sensor read interval (in seconds)
const long readInterval = 80;   // 60s

//MQTT
#define MQTT_SERVER "10.0.1.3"
#define MQTT_PORT 1883

//MQTT
const char* mqttServer = MQTT_SERVER;
int mqttPort = MQTT_PORT;

//Other MQTT things
String espName = ESPNAME;
String mqttBaseTopic = String("casa");
