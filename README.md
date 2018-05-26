# ESP8266 meteorolical station

Meteorological station with humidity, temperature, barometer, CO2, light intensity and UV intensity. Time is sinchronized unsing an NTP server. It publish the sensors readings into an MQTT server.

Web interface lets you connect to your wifi and see sensor readings.

In this version you will find:
- New Web interface, now responsive.
- OTA firmware update.
- Configuration AP protected with password.
- New debug mode.
- Openscad files to print the station enclosure.
- Now web interface shows uptime and current time.
- Fallback to AP mode in case of loosing access to AP.
- Tons of code and circuit testing.
- Added circuit schema.

Seccurity issues
- MANY....
- No support for ssl nor tls, or any other form of encription.
- No password support to access web interface.
- ETC..

# Configuration

There are a few parameters that can be changed, they are placed at config.h file:
 * Pinset to define the pins sensors are connected to.
 * AP user and password.
 * MQTT server address.
 * NTP configuration.
 * OTA Username, password and URL.
 * Reading interval of the sensors.

# Screenshots

## Web interface

![index](https://raw.githubusercontent.com/carlosmaug/meteoESP8266/master/screenshots/index.png)

# Hardware

TODO: Include schemas

## Dependencies

This application is based on many libraries:

- [NtpClientLib](https://github.com/arduino-libraries/NTPClient)
- [ESP8266WebServer](https://github.com/esp8266/Arduino/tree/master/libraries)
- [ESP8266HTTPUpdateServer](https://github.com/esp8266/Arduino/tree/master/libraries)
- [ESP8266mDNS](https://github.com/esp8266/Arduino/tree/master/libraries)
- [ESP8266WiFi](https://github.com/esp8266/Arduino/tree/master/libraries)
- [WiFiManager](https://github.com/tzapu/WiFiManager)
- [DHT.h](https://github.com/adafruit/DHT-sensor-library)
- [BH1750.h](https://github.com/claws/BH1750)
- [SFE_BMP180.h](https://github.com/LowPowerLab/SFE_BMP180)
- [MQ135.h](https://github.com/GeorgK/MQ135)
- [FS.h](https://github.com/esp8266/Arduino)
- [PubSubClient](https://github.com/knolleary/pubsubclient)


# Licenses

This code is under GPL3 license
