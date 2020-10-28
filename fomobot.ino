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

//Start Shifer
MqttClient mqttClient(wifiClient);

const char broker[] = "broker.shiftr.io";
int        port     = 1883;
const char topic[]  = "/wollio/fomobot";
//End Shifter

//NTP (Network Time Protocol)
NTPClient timeClient(ntpUDP);

//Printer
SoftwareSerial printerSerial(PRINT_ID_RXD, PRINT_ID_TXD);
Adafruit_Thermal printer(&printerSerial);     // Pass addr to printer constructor

int lastPrint = 0;
int nextSpeak = 0;
int lastKeepAlive = 0;

void setup() {
  Serial.begin(LSS_BAUD);
  Serial.println(LSS_DefaultBaud);
  Serial.println("hello");

  //Wait for WiFi to connect
  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  driver = Driver(servoSerial);
  toner = Toner(SPEAKER_PIN_ID);

  // You can provide a unique client ID, if not set the library uses Arduino-millis()
  // Each client must have a unique client ID
  // mqttClient.setId("clientId");

  // You can provide a username and password for authentication
  mqttClient.setUsernamePassword(MQTT_KEY, MQTT_PWD);

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  Serial.print("Subscribing to topic: ");
  Serial.println(topic);
  Serial.println();

  // set the message receive callback
  mqttClient.onMessage(onMqttMessage);

  // subscribe to a topic
  mqttClient.subscribe(topic);

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
  mqttClient.poll();

  // publish a message roughly every second.
  if(millis() > lastKeepAlive) {
    Serial.println("Send mqtt");
    lastKeepAlive = millis() + 10000;
    mqttClient.beginMessage(topic);
    mqttClient.print(driver.state);
    mqttClient.endMessage();
  }

  if (lastPrint < millis() - 10000) {
    Serial.println("print");
    // Barcode examples:
    // CODE39 is the most common alphanumeric barcode:
    //printer.setBarcodeHeight(100);
    //printer.printBarcode("ADAFRUT", CODE39);
  }

  if (millis() > nextSpeak) {
    Serial.println("speak");
    driver.stopCar();
    toner.speak(); 
    nextSpeak = millis() + random(10000, 20000);
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

  if (random(0,10) > 9) {
    driver.shake();
  } else {
    driver.drive();
  }
}

void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  // use the Stream interface to print the contents
  String payload = "";
  while (mqttClient.available()) {
    payload = payload + (char)mqttClient.read();
  }
  Serial.println(payload);
  // move servo motor
  // myLSS.move(payload.toInt());
}

int convertSonicSensorValueToCm(int sensorValue) {
  return (sensorValue / 2) / 29.1;
}

int readSonicSensor(int echoSonicSensorId) {
  int val = pulseIn(echoSonicSensorId, HIGH);
  if (val < 0 || val > 3000) {
    val = 3000;
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
