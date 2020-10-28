#include <NTPClient.h>
#include <WiFiUdp.h>

#include <Bridge.h>
#include <HttpClient.h>
#include <SoftwareSerial.h>
#include <Adafruit_Thermal.h>

#include <ArduinoMqttClient.h>

//Everything related to WiFi
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
#include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
#include <WiFi101.h>
#elif defined(ARDUINO_ESP8266_ESP12)
#include <ESP8266WiFi.h>
#endif
