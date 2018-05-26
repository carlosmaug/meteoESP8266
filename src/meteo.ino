// NTP
#include <NtpClientLib.h>

// Web server an OTA support
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>

// FS support for config sotre
#include <FS.h>

// WIFI Support
#include <ESP8266WiFi.h>
#include <WiFiManager.h>

// MQTT
#include <PubSubClient.h>

//Sensor support
#include <Wire.h>
#include <DHT.h>
#include <BH1750.h>
#include <SFE_BMP180.h>
#include <MQ135.h>

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
#define DEBUG  false

//-----------------------------------------------------------------------
//--------------- GENARAL CONFIGURATION PARAMETERS ----------------------
//-----------------------------------------------------------------------

// Wifi AP 
// This AP is strarted if we cant connect to the WIFI
char apName[12] = "AP-Name";
char apPass[13] = "AP-pass";

//NTP
char ntpServer[100] = "192.168.1.1";

// Interval between ntp updates (in milliseconds)
uint16_t ntpint = 600; 
const long ntpInterval = 3600000;  // 1h

// Sensor read interval (in milliseconds)
const long readInterval = 60000;   // 60s

//MQTT
const char* mqttServer = "192.168.1.1";
int mqttPort = 1883;
long lastMsg = 0;
char msg[50];
int value = 0;

//Other MQTT things
String espName = String("Meteo_1");
String mqttBaseTopic = String("casa");

//OTA
//const char* host = espName;
const char* updatePath = "/firmware";
const char* updateUsername = "admin";
const char* updatePassword = "password";

//------------------------------------------
//---- Sensor and other initialitations ----
//------------------------------------------
SFE_BMP180 bmp;

BH1750 light;

// Initialize DHT sensor 
// NOTE: For working with a faster than ATmega328p 16 MHz Arduino chip, like an ESP8266,
// you need to increase the threshold for cycle counts considered a 1 or 0.
// You can do this by passing a 3rd parameter for this threshold.  It's a bit
// of fiddling to find the right value, but in general the faster the CPU the
// higher the value.  The default for a 16mhz AVR is a value of 6.  For an
// Arduino Due that runs at 84mhz a value of 30 works.
// This is for the ESP8266 processor on ESP-01 
DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266

WiFiClient espClient;

PubSubClient mqtt(espClient);

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;

//MQ135
MQ135 gasSensor = MQ135(ANALOGPIN);

//----------------------------------------------------
//------  Global variables for sensor readings -------
//----------------------------------------------------
double temp, pres, tempMin = 99, presMin = 999999.99, tempMax = -99, presMax = 0;
float uv, humidity, dhtTemp;
float uvMax = 0, luxMax = 0, humidityMax = 0, dhtTempMax = -99, co2Max = 0;
float uvMin = 0, luxMin = 0, humidityMin = 100, dhtTempMin = 99, co2Min = 9999.99;

struct co2read {
  float ppm;
  float ppmCor;
};

uint16_t lux;

struct co2read co2;

// Initian values for corrected CO2 measurements
float co2CorMin = 999999.99, co2CorMax = 0;

//Gloval variables for web pages
char str[2490],cTemp[8], cHum[8], cPres[8], cLux[8], cUv[8], cCo2[8], cCo2Cor[8];
char cTempMin[8], cHumMin[8], cPresMin[8], cLuxMin[8], cUvMin[8], cCo2Min[8], cCo2CorMin[8];
char cTempMax[8], cHumMax[8], cPresMax[8], cLuxMax[8], cUvMax[8], cCo2Max[8], cCo2CorMax[8]; 

// Aux var for reading intervals (used in readSensors function)
unsigned long lastRead = 0;

// Aux used to detect if WIFI gets a new IP
bool wifiFirstConnected = true;

bool syncEventTriggered = false; // True if a time even has been triggered
NTPSyncEvent_t ntpEvent; // Last triggered event

//-------------------------------------------------------------------------------

void setup() {
  static WiFiEventHandler e1;
  
  // Start Serial for debugging on the Serial Monitor
  Serial.begin(115200);  

  // Connect to WIFI
  WiFiManager wifiManager;
  wifiManager.autoConnect(apName, apPass);
  wifiManager.setConfigPortalTimeout(30);
  if (DEBUG) wifiManager.setDebugOutput(DEBUG);
  e1 = WiFi.onStationModeGotIP (onWIFIGotIP);// As soon WiFi is connected, start NTP Client

  //------------ Sensors --------------------------
  //IO dedicated to SDA, SLC
  Wire.begin(SDAPIN, SLCPIN);

  //IO dedicated to power ML8511
  pinMode(MLPIN,OUTPUT);
  digitalWrite(MLPIN, LOW);
 
  //IO dedicated to power MQ135
  pinMode(MQPIN,OUTPUT);
  digitalWrite(MQPIN, LOW);

  Serial.println("");
   
  // Initialize the sensor (it is important to get calibration values stored on the device).
  if ( bmp.begin() ) {
    Serial.println("BMP180 init success");  
  } else {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

    Serial.println("BMP180 init fail");
  }
  
  light.begin();
  if (DEBUG) Serial.println("BH1750 init success");
  
  dht.begin();
  if (DEBUG) Serial.println("DHT init success");

  pinMode(ANALOGPIN, INPUT);
  //------------ Sensors end --------------------------
  
  // HTTP Server Start
  httpUpdater.setup(&server, updatePath, updateUsername, updatePassword);
  if (DEBUG) Serial.printf("HTTPUpdateServer ready! Open http://%s.local%s in your browser and login with username '%s' and password '%s'\n", espName.c_str(), updatePath, updateUsername, updatePassword);


  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

  // MSDN Initialitation
  if (!MDNS.begin((const char*)espName.c_str())) {
    Serial.println("Error setting up MDNS responder!");
  }
  
  if (DEBUG) Serial.println("mDNS responder started");
  
  
  // Init update service autodiscover
  MDNS.addService("http", "tcp", 80);
  
  // NTP define events
  NTP.onNTPSyncEvent([](NTPSyncEvent_t event) {
    ntpEvent = event;
    syncEventTriggered = true;
  });

  // MQTT client start
  mqtt.setServer(mqttServer, mqttPort);
  mqtt.setCallback(mqttCallback);
  mqttBaseTopic = mqttBaseTopic + "/" + espName;

}

void loop() {
  static int lastNtp = 0;

  if (wifiFirstConnected) {
    wifiFirstConnected = false;
    NTP.begin (ntpServer, 1, true);
    NTP.setInterval (63);
  }

  if (syncEventTriggered) {
    ntpSyncEvent (ntpEvent);
    syncEventTriggered = false;
  }
 
  //MQTT client
  if (!mqtt.connected()) {
    mqttReconnect();
  }
  
  mqtt.loop();
  readSensors(readInterval);
  server.handleClient();

  if (DEBUG && ((millis() - lastNtp) > readInterval || lastNtp == 0)) {
    lastNtp = millis();
    Serial.print(NTP.getTimeDateString()); Serial.print(". ");
    Serial.print("Uptime: ");
    Serial.print(NTP.getUptimeString()); Serial.print(" since ");
    Serial.println(NTP.getTimeDateString(NTP.getFirstSync()).c_str());
  }
}

/**
 * @brief This function is called when WIFI gots IP
 */
void onWIFIGotIP (WiFiEventStationModeGotIP ipInfo) {
    wifiFirstConnected = true;
}

/**
 * @brief This function is called when an NTP syncronization event is trigered
 */
void ntpSyncEvent (NTPSyncEvent_t ntpEvent) {
    if (ntpEvent) {
        Serial.print ("Time Sync error: ");
        if (ntpEvent == noResponse)
            Serial.println ("NTP server not reachable");
        else if (ntpEvent == invalidAddress)
            Serial.println ("Invalid NTP server address");
    } else {
        if (DEBUG) Serial.print ("Got NTP time: ");
        if (DEBUG) Serial.println (NTP.getTimeDateString (NTP.getLastNTPSync ()));
    }
}

/**
 * @brief Calculate de avereje value of the analog readings.
 * @param pinToRead Analog pin to read.
 * @param numberOfReadings Number of readings to take in consideration. 
 * @return Averaje value.
 * 
 */
int averageAnalogRead(int pinToRead, int numberOfReadings = 8 ) {
  unsigned int runningValue = 0; 

  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
      
  runningValue /= numberOfReadings;

  return(runningValue);  
}

/**
 * @brief Read MQ135 CO2 gas sensor.
 * @param pin Pin used to activate sensor.
 * @param delay Delay until sensor is readed (preheating).
 * @param temp Hambient temperature.
 * @param hum Hambient humidity.
 * @return CO2 ppm.
 * 
 * Before start reading the sensor you must calibrate it. 
 * This function returns the CO2 concentration corrected
 * with the humidity and temperature values.
 */
struct co2read co2Read(int pin, int dalay, float temp, float hum) {
  struct co2read ppm;
  
  digitalWrite(pin, HIGH);
  delay(dalay); 
  ppm.ppm = gasSensor.getPPM();
  ppm.ppmCor = gasSensor.getCorrectedPPM(temp, hum);
  
  digitalWrite(pin, LOW);
  return ppm;
}

/**
 * @brief Read ML8511 UV sensor. 
 * @param pin Pin used to activate sensor.
 * @param delay Delay until sensor is readed.
 * @return UV mW/cm2.
 * 
 * This function returns the UV intensity in mW/cm2
 */
float uvRead(int pin, int dalay) {
  digitalWrite(pin, HIGH);
  delay(dalay); 
  int uvLevel = averageAnalogRead(ANALOGPIN);
  digitalWrite(pin, LOW);
  return mapfloat((float)uvLevel*6.2/1000, 0.3968, 4.8, 0.0, 15.00); //Convert the voltage to a UV intensity level
}

/**
 * @brief Read DHT humidiy/temperature sensor. 
 * @return Array Temperature - Humidity.
 * 
 * This function returns an array with the temperature in celsius degrees
 * and the relative humidiy in %
 */
float* dhtRead () {
  float dat[2];
  int count = 0;
  
  do {
    dat[0] = dht.readTemperature();
    dat[1] = dht.readHumidity();  
    count++;
  } while (count < 20 && (isnan(dat[0]) || isnan(dat[1])));

  if (count < 20) {
    return dat;
  }
  
  Serial.println("Failed to read from DHT sensor!");
  return dat;
}

/**
 * @brief Read BMP180 preasure/temperature sensor. 
 * @return Array Temperature - preasure.
 * 
 * This function returns an array with the temperature in celsius degrees
 * and the preasure in mb
 */
double* bmpRead () {
  double dat[2];
  char status;

  // Retrieve the completed pressure measurement:
  // Note that the measurement is stored in the variable P.
  // Note also that the function requires the previous temperature measurement (T).
  // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
  // Function returns 1 if successful, 0 if failure.
  status = bmp.startTemperature();
    
  if (status != 0) {
    // Wait for the measurement to complete:
    delay(status);
    status = bmp.getTemperature(dat[0]);

    if (status != 0) {
      status = bmp.startPressure(3);

      if (status != 0) {
        // Wait for the measurement to complete:
        delay(status);
        status = bmp.getPressure(dat[1], dat[0]);

        if (status == 0) {
          Serial.println("BMP: error retrieving pressure measurement\n");
        }
      } else {
        Serial.println("BMP: error starting preasure measurement\n");
      }
    } else {
      Serial.println("BMP: error reading temperature measurement\n");
    }
  } else {
    Serial.println("BMP: error starting temperature measurement\n");
  }
 
  return dat;
}

/**
 * @brief Read sensor values, waiting a certain interval between readings.
 * @param interval Interval between readings
 *
 */
void readSensors(unsigned long interval) {
  float* dht;
  double* bmp;

  if ((millis() - lastRead) > interval || lastRead == 0) {
    lastRead = millis();

    //DHT Readings
    dht = dhtRead();
    
    if ( !isnan(dht[0]) ) {
      dhtTemp = dht[0];
      humidity = dht[1]; 
    }
    
    //MQ135
    co2 = co2Read(MQPIN, MQDELAY, dhtTemp, humidity);

    // BMP180
    bmp  = bmpRead();
    temp = bmp[0];
    pres = bmp[1];

    //BH1750
    lux = light.readLightLevel();
    //Lux to Watts/m2
    lux = lux/683.0;

    //ML8511
    uv = uvRead(MLPIN, MLDELAY);

    if (tempMin > temp) tempMin = temp;
    if (tempMax < temp) tempMax = temp;
    if (presMin > pres) presMin = pres;
    if (presMax < pres) presMax = pres;
    
    if (dhtTempMin > dhtTemp) dhtTempMin = dhtTemp;
    if (dhtTempMax < dhtTemp) dhtTempMax = dhtTemp;
    if (humidityMin > humidity) humidityMin = humidity;
    if (humidityMax < humidity) humidityMax = humidity;

    if (luxMin > lux) luxMin = lux;
    if (luxMax < lux) luxMax = lux;

    if (uvMin > uv) uvMin = uv;
    if (uvMax < uv) uvMax = uv;
    
    if (co2Min > co2.ppm) co2Min = co2.ppm;
    if (co2Max < co2.ppm) co2Max = co2.ppm;

    if (co2CorMin > co2.ppmCor) co2CorMin = co2.ppmCor;
    if (co2CorMax < co2.ppmCor) co2CorMax = co2.ppmCor;
    
    dtostrf(pres,4, 2, cPres);
    dtostrf(presMax,4, 2, cPresMax);
    dtostrf(presMin,4, 2, cPresMin);
    
    dtostrf(humidity,3, 2, cHum);
    dtostrf(humidityMax,3, 2, cHumMax);
    dtostrf(humidityMin,3, 2, cHumMin);
    
    dtostrf(dhtTemp,3, 2, cTemp);
    dtostrf(dhtTempMax,3, 2, cTempMax);
    dtostrf(dhtTempMin,3, 2, cTempMin);
    
    dtostrf(lux,3, 2, cLux);
    dtostrf(luxMax,3, 2, cLuxMax);
    dtostrf(luxMin,3, 2, cLuxMin);

    // 10 is the scale factor form mW/cm2 to W/m2
    dtostrf(uv*10,3, 2, cUv);
    dtostrf(uvMax*10,3, 2, cUvMax);
    dtostrf(uvMin*10,3, 2, cUvMin);

    dtostrf(co2.ppm,3, 2, cCo2);
    dtostrf(co2Max,3, 2, cCo2Max);
    dtostrf(co2Min,3, 2, cCo2Min);

    dtostrf(co2.ppmCor,3, 2, cCo2Cor);
    dtostrf(co2CorMax,3, 2, cCo2CorMax);
    dtostrf(co2CorMin,3, 2, cCo2CorMin);

    mqttPublishValues ();
          
    // Print out the measurement
    if ( DEBUG ) {
      Serial.print("UV: "); Serial.print(uv*10); Serial.print(" W/m2, ");
      Serial.print("Intensidada Solar: "); Serial.print(lux); Serial.print(" W/m2, ");
      Serial.print("Humidity: "); Serial.print(humidity); Serial.print(" %, ");
      Serial.print("DHT Temperature: "); Serial.print(dhtTemp); Serial.print(" C, ");
      Serial.print("temperature: "); Serial.print(temp,2); Serial.print(" ºC, ");
      Serial.print("Pressure: "); Serial.print(pres,2); Serial.print(" hPa, ");
      Serial.print("CO2: "); Serial.print(co2.ppm); Serial.print(" ppm, ");
      Serial.print("CO2 corregido: "); Serial.print(co2.ppmCor); Serial.println(" ppm");
    }
  }
}

/**
 * @brief The Arduino Map function but for floats.
 * @param x Value to map.
 * @param inMin Minimun input value possible. 
 * @param inMax Maximun input value possible. 
 * @param outMin Minimun output value possible. 
 * @param outMax Maximun output value possible. 
 * @return Mapped value.
 * 
 * From: http://forum.arduino.cc/index.php?topic=3 922.0
 */
float mapfloat(float x, float inMin, float inMax, float outMin, float outMax)
{
  float result = (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
  if (result > 0) return result;
  return 0;
}

//MQTT new message from broker
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  if ( DEBUG ) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    
    Serial.println();
  }
}

//MQTT Reconnect
void mqttReconnect() {
  // Loop until we're reconnected
  char espNameC[50]; 
  espName.toCharArray(espNameC,espName.length()+1);
  
  while (!mqtt.connected()) {
    if (DEBUG) Serial.print("Attempting MQTT connection... ");
    // Attempt to connect
    if (mqtt.connect(espNameC)) {
      if (DEBUG) Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//MQTT send message
void mqttSendMessage (String topic, char* playload) {
  //MQTT Message
  char mqttTopicC[256];
  topic = mqttBaseTopic + "/" + topic;
  topic.toCharArray(mqttTopicC,topic.length()+1);

  if (DEBUG) {
    Serial.print("Publish - Topic:");
    Serial.print(mqttTopicC);
    Serial.print(" -> message: ");
    Serial.println(playload);  
  }
  
  mqttReconnect();

  mqtt.publish(mqttTopicC, playload);
  
}

//MQTT publish values
void mqttPublishValues () {
  mqttSendMessage (String("presion"), cPres);
  mqttSendMessage (String("humedad"), cHum);
  mqttSendMessage (String("temperatura"), cTemp);
  mqttSendMessage (String("radiacion"), cLux);
  mqttSendMessage (String("ultravioleta"), cUv); 
  mqttSendMessage (String("co2"), cCo2);
  mqttSendMessage (String("co2Cor"), cCo2Cor);
}

String getContentType(String filename) {
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".svg")) return "image/svg";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

//Red files from FS
bool handleFileRead (String path) {
  if(path.endsWith("/")) path += "index.html";
  
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";

    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  
  return false;
}


/**
 * @brief Read WiFi connection information from file system.
 * @param ssid String pointer for storing SSID.
 * @param pass String pointer for storing PSK.
 * @return True or False.
 * 
 * The config file have to containt the WiFi SSID in the first line
 * and the WiFi PSK in the second line.
 * Line seperator can be \r\n (CR LF) \r or \n.
 */
bool loadConfig(String *ssid, String *pass)
{
  // open file for reading.
  File configFile = SPIFFS.open("/conf.txt", "r");
  if (!configFile) {
    Serial.println("Failed to open conf.txt.");

    return false;
  }

  // Read content from config file.
  String content = configFile.readString();
  configFile.close();
  
  content.trim();

  // Check if ther is a second line available.
  int8_t pos = content.indexOf("\r\n");
  uint8_t le = 2;
  // check for linux and mac line ending.
  if (pos == -1) {
    le = 1;
    pos = content.indexOf("\n");
    
    if (pos == -1) {
      pos = content.indexOf("\r");
    }
  }

  // If there is no second line: Some information is missing.
  if (pos == -1) {
    Serial.println("Invalid content.");
    Serial.println(content);

    return false;
  }

  // Store SSID and PSK into string vars.
  *ssid = content.substring(0, pos);
  *pass = content.substring(pos + le);

  ssid->trim();
  pass->trim();

  if (DEBUG) {
    Serial.println("----- file content -----");
    Serial.println(content);
    Serial.println("----- file content -----");
    Serial.println("ssid: " + *ssid);
    Serial.println("psk:  " + *pass);
  }

  return true;
} // loadConfig

/**
 * @brief Save WiFi SSID and PSK to configuration file.
 * @param ssid SSID as string pointer.
 * @param pass PSK as string pointer,
 * @return True or False.
 */
bool saveConfig(String *ssid, String *pass)
{
  // Open config file for writing.
  File configFile = SPIFFS.open("/conf.txt", "w");
  if (!configFile)  {
    Serial.println("Failed to open conf.txt for writing");
    return false;
  }

  // Save SSID and PSK.
  configFile.println(*ssid);
  configFile.println(*pass);

  configFile.close();
  
  return true;
} // saveConfig




String datossWeb (String titulo, String unidad, char* dato, char* max, char* min, String imgName) {
  return String("<div class=caja>\
<div class=dato>\
<div class=img>\
<img src='") + imgName + String("' alt=' '>\
</div>\
<p class=titulo>") + titulo + String(":</p>\
<p class=dato>") + dato + " " + unidad + String("</p>\
</div>\
<div class=tabla>\
<div class=fila>\
<div class=celda>Máx: ") + max + String("</div>\
<div class=celda>Mín: ") + min + String("</div>\
</div>\
</div>\
</div>");

}

void handleRoot() {
  String html;
  String cabecera = String("<!DOCTYPE html>\
<html lang=es-ES>\
<head>\
<title>Datos meteorológicos</title>\
<meta charset=UTF-8>\
<style>div.caja{background-color:#005ac0;color:#fff;margin:2px auto;padding:20px;width:27%;min-width:170px;border-radius:15px;border-width:3px;border-color:#bff8ff;border-style:solid;min-height:103px;display:inline-block;text-align:left}div.img{float:left;height:112px}div.contenedor{max-width:93%;margin:0 auto;text-align:center}div.dato{min-height:112px}p.titulo{margin:0 auto 8px;font-size:26px}p.dato{font-size:30px;font-weight:700;margin:0 auto 8px}.tabla{display:table;width:100%;text-align:center}.fila{display:table-row}.celda{font-size:20px;display:table-cell}img{margin-right:25px}.pagina{background:linear-gradient(#003060,#09b8fc);max-width:90%;min-height:700px;border-radius:20px;margin:auto;padding:2%}body{font-family:arial,sans-serif}</style>\
</head>\
<body>\
<div class=pagina>\
<h1 style=text-align:center;color:aliceblue>Datos meteorológicos</h1>\
<h2 style=text-align:center;color:aliceblue>Hora: ") + NTP.getTimeDateString() + String("</h2>\
<h2 style=text-align:center;color:aliceblue>Arriba:") + NTP.getUptimeString() + String("</h2>\
<div class=contenedor>\n");

  String pie = "</div>\
</body>\
</html>";

  html = cabecera + datossWeb("Temperatura exterior", "ºC", cTemp, cTempMax, cTempMin, "temp.svg");
  html = html + datossWeb("Humedad ambiente", "%", cHum, cHumMax, cHumMin, "humedad.svg");
  html = html + datossWeb("Presión atmosférica", "hPa", cPres, cPresMax, cPresMin, "presion.svg");
  html = html + datossWeb("Radiación UV", "W/m<sup>2</sup>", cUv, cUvMax, cUvMin, "uv.svg");
  html = html + datossWeb("Radiación solar", "W/m<sup>2</sup>", cLux, cLuxMax, cLuxMin, "solar.svg");
  html = html + datossWeb("CO<sub>2</sub>", "ppm", cCo2, cCo2Max, cCo2Min, "solar.svg");
  html = html + pie;

  server.send ( 200, "text/html", html );

}

void handleNotFound(){
  if(!handleFileRead(server.uri())) {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET)?"GET":"POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    
    for (uint8_t i = 0; i < server.args(); i++) {
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    
    server.send(404, "text/plain", message);
  }
}
