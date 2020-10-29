#include "libs.h"
#include "config.h"

#include "Driver.h"
#include "Toner.h"

#define eventIntervalMin 5000
#define eventIntervalMax 10000

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
// Printer RX, TX (on Board)
SoftwareSerial printerSerial(PRINT_ID_RXD, PRINT_ID_TXD);
Adafruit_Thermal printer(&printerSerial, A5);     // Pass addr to printer constructor

float lastPrint = 0;
float getNextEventAt = 0.0;
float speakNextAt = 0;
int lastKeepAlive = 0;
bool eyeOn = false;

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


  Serial.println("hello1.5");

  // NOTE: SOME PRINTERS NEED 9600 BAUD instead of 19200, check test page.
  printerSerial.begin(9600);  // Initialize SoftwareSerial
  //Serial1.begin(19200); // Use this instead if using hardware serial
  printer.begin();        // Init printer (same regardless of serial type)

  //Start NTP time request
  timeClient.begin();

  printerSerial.begin(9600);  // Initialize SoftwareSerial
  printer.wake();       // MUST wake() before printing again, even if reset
  printer.setDefault(); // Restore printer to defaults

  //Setup sonic sensor pins
  pinMode(SS_LEFT_ID_TRIG, OUTPUT);
  pinMode(SS_LEFT_ID_ECHO, INPUT);
  pinMode(SS_CENT_ID_TRIG, OUTPUT);
  pinMode(SS_CENT_ID_ECHO, INPUT);
  pinMode(SS_RIGH_ID_TRIG, OUTPUT);
  pinMode(SS_RIGH_ID_ECHO, INPUT);

  pinMode(EYE_PIN_ID, OUTPUT);

  pinMode(A5, OUTPUT);
}

void toogleEye() {
  eyeOn = !eyeOn;
}

void getEvent() {
  getNextEventAt = (millis() + random(eventIntervalMin, eventIntervalMax)) / 1000;
  sendMqttMessage("getEvent");
}

void sendMqttMessage(String msg) {
  mqttClient.beginMessage(topic);
  mqttClient.print(msg);
  mqttClient.endMessage();
}

void loop() {
  analogWrite(A5, HIGH);

  Serial.println("hello2");

  // Move the LSS continuously in one direction
  mqttClient.poll();

  if ((millis() / 1000) > speakNextAt) {
    Serial.println("speak");
    driver.stopCar();
    driver.headDown();
    toner.speak();
    driver.headUp();
    speakNextAt = (millis() + random(10000, 20000)) / 1000;
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

  if (random(0, 10) > 9) {
    driver.shake();
  } else {
    driver.drive();
  }

  if (getNextEventAt < (millis() / 1000)) {
    getEvent();
  }

}

String getSplitVal(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  // use the Stream interface to print the contents
  String payload = "";
  while (mqttClient.available()) {
    payload = payload + (char)mqttClient.read();
  }

  
  
  Serial.println(payload);

  Serial.println("print");
  //printTheFuck();

  driver.cut();
  //printer.feed(2);
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

void printTheFuck(String msg, String date) {
  Serial.println("printing...");

  printer.setLineHeight(60);
  printer.justify('C');
  printer.setSize('L');
  printer.setLineHeight(40);
  printer.println(F("     *     "));
  printer.println(F(msg));
  printer.println(F(msg));
  printer.println(F(msg));
  printer.println(F("     *     "));

  printer.setSize('L');
  printer.println(F("4K22"));
  printer.println(F(date));
  printer.println(F("13:00"));

  printer.setLineHeight(60);
  printer.justify('C');
  printer.setSize('M');

  printer.setLineHeight();
  printer.println(F("ZHdK"));
  printer.println(F("ZURICH"));


  printer.setSize('L');
  printer.setLineHeight(40);
  printer.println(F("*"));
  printer.println(F("BE THERE!"));
  printer.println(F("*"));

  printer.setLineHeight(60); // Cut Spacing
  printer.justify('C');
  printer.setSize('L');
  printer.println(F(" "));
  printer.println(F(" "));
  printer.println(F(" "));
  printer.println(F(" "));
  printer.feed(10);

  printer.sleep();      // Tell printer to sleep
  delay(3000L);         // Sleep for 3 seconds
  printer.wake();       // MUST wake() before printing again, even if reset
  printer.setDefault(); // Restore printer to defaults
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
