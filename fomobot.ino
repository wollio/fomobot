#include "libs.h"
#include "config.h"

#include "Driver.h"
#include "Toner.h"

//LynxMotion TX / RX
SoftwareSerial servoSerial(8, 9);
Driver driver;
Toner toner;

char ssid[] = SECRET_SSID;        // your network SSID (name)
char password[] = SECRET_PASS;        // your network password (use for WPA, or use as key for WEP)

WiFiClient wifiClient;
WiFiUDP ntpUDP;

//End Wifi

//NTP (Network Time Protocol)
NTPClient timeClient(ntpUDP);

//Printer
SoftwareSerial printerSerial(PRINT_ID_RXD, PRINT_ID_TXD);
Adafruit_Thermal printer(&printerSerial);     // Pass addr to printer constructor

int lastPrint = 0;

void setup() {

  Serial.begin(LSS_BAUD);
  Serial.println(LSS_DefaultBaud);
  Serial.println("hello");

  driver = Driver(servoSerial);
  toner = Toner(SPEAKER_PIN_ID);

  //Wait for WiFi to connect
  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  // NOTE: SOME PRINTERS NEED 9600 BAUD instead of 19200, check test page.
  printerSerial.begin(9600);  // Initialize SoftwareSerial
  //Serial1.begin(19200); // Use this instead if using hardware serial
  printer.begin();        // Init printer (same regardless of serial type)

  //Start NTP time request
  timeClient.begin();

  //Setup sonic sensor pins
  pinMode(SS_LEFT_ID_TRIG, OUTPUT);
  pinMode(SS_LEFT_ID_ECHO, INPUT);
  pinMode(SS_CENT_ID_TRIG, OUTPUT);
  pinMode(SS_CENT_ID_ECHO, INPUT);
  pinMode(SS_RIGH_ID_TRIG, OUTPUT);
  pinMode(SS_RIGH_ID_ECHO, INPUT);
}

void loop() {
  // Move the LSS continuously in one direction

  if (lastPrint < millis() - 10000) {
    Serial.println("print");
    // Barcode examples:
    // CODE39 is the most common alphanumeric barcode:
    //printer.setBarcodeHeight(100);
    //printer.printBarcode("ADAFRUT", CODE39);
  }

  trigSonicSensor(SS_LEFT_ID_TRIG);
  driver.distanceLeft = readSonicSensor(SS_LEFT_ID_ECHO);
  trigSonicSensor(SS_CENT_ID_TRIG);
  driver.distanceFront = readSonicSensor(SS_CENT_ID_ECHO);
  trigSonicSensor(SS_RIGH_ID_TRIG);
  driver.distanceRight = readSonicSensor(SS_RIGH_ID_ECHO);

  driver.distanceLeft = convertSonicSensorValueToCm(driver.distanceLeft);
  driver.distanceFront = convertSonicSensorValueToCm(driver.distanceFront);
  driver.distanceRight = convertSonicSensorValueToCm(driver.distanceRight);

  Serial.print(driver.distanceLeft);
  Serial.print(" ");
  Serial.print(driver.distanceFront);
  Serial.print(" ");
  Serial.println(driver.distanceRight);

  driver.drive();

  Serial.println("speak");
  toner.speak();
}

int convertSonicSensorValueToCm(int sensorValue) {
  return (sensorValue / 2) / 29.1;
}

int readSonicSensor(int echoSonicSensorId) {
  int val = pulseIn(echoSonicSensorId, HIGH);
  if (val < 0) {
    val = 999;
  }
  return val;
}

/**

   This triggers the sonic sensor
*/
void trigSonicSensor(int triggerSonicSensorId) {
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(triggerSonicSensorId, LOW);
  delayMicroseconds(5);
  digitalWrite(triggerSonicSensorId, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerSonicSensorId, LOW);
}
