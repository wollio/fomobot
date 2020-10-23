#include <LSS.h>
#include <SoftwareSerial.h>
SoftwareSerial servoSerial(8, 9);

// ID set to default LSS ID = 0
#define LSS_ID_old (254)

#define LSS_ID0    (0)
#define LSS_ID1    (1)

#define LSS_BAUD  (LSS_DefaultBaud)

#define SS_LEFT_ID_TRIG 2
#define SS_LEFT_ID_ECHO 3
#define SS_CENT_ID_TRIG 6
#define SS_CENT_ID_ECHO 7
#define SS_RIGH_ID_TRIG 4
#define SS_RIGH_ID_ECHO 5

// Create one LSS object
LSS myLSS0 = LSS(LSS_ID0);
LSS myLSS1 = LSS(LSS_ID1);


void setup() {

  //Setup sonic sensor pins
  pinMode(SS_LEFT_ID_TRIG, OUTPUT);
  pinMode(SS_LEFT_ID_ECHO, INPUT);
  pinMode(SS_CENT_ID_TRIG, OUTPUT);
  pinMode(SS_CENT_ID_ECHO, INPUT);
  pinMode(SS_RIGH_ID_TRIG, OUTPUT);
  pinMode(SS_RIGH_ID_ECHO, INPUT);
  
  servoSerial.begin(LSS_BAUD);
  // Initialize the LSS bus
  LSS::initBus(servoSerial, LSS_BAUD);
  Serial.begin(LSS_BAUD);
}

void loop() {
  // Move the LSS continuously in one direction
  myLSS0.wheelRPM(-0);
  myLSS1.wheelRPM(0);

  trigSonicSensor(SS_LEFT_ID_TRIG);
  int val1 = readSonicSensor(SS_LEFT_ID_ECHO);
  trigSonicSensor(SS_CENT_ID_TRIG);
  int val2 = readSonicSensor(SS_CENT_ID_ECHO);
  trigSonicSensor(SS_RIGH_ID_TRIG);
  int val3 = readSonicSensor(SS_RIGH_ID_ECHO);

  val1 = convertSonicSensorValueToCm(val1);
  val2 = convertSonicSensorValueToCm(val2);
  val3 = convertSonicSensorValueToCm(val3);

  Serial.print(val1);
  Serial.print(" ");
  Serial.print(val2);
  Serial.print(" ");
  Serial.println(val3);
}

int convertSonicSensorValueToCm(int sensorValue) {
  return (sensorValue/2) / 29.1;  
}

int readSonicSensor(int echoSonicSensorId) {
  return pulseIn(echoSonicSensorId, HIGH);
}

/**
 * 
 * This triggers the sonic sensor
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
