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

#define MAX_SPEED 100

int currentSpeed = 0;
int desiredSpeed = 100;

int distanceRight = 0;
int distanceLeft = 0;
int distanceFront = 0;

const int minFrontDistance = 30;
const int minSideDistance = 20;
const int stuckDistance = 10;

const int delayTime = 150;


// Create one LSS object
LSS leftWheel = LSS(LSS_ID0);
LSS rightWheel = LSS(LSS_ID1);

void setRightWheel(int speeed) {
  //invert one value
  rightWheel.wheelRPM(speeed * -1);
}

void setLeftWheel(int speeed) {
  leftWheel.wheelRPM(speeed);
}

void stopCar () {
  setRightWheel(0);
  setLeftWheel(0);
}

void goFull() {
  desiredSpeed = MAX_SPEED;
  currentSpeed = currentSpeed + round((desiredSpeed - currentSpeed) * 0.1);
  Serial.print("Go Full");
  Serial.println(currentSpeed);
  rightWheel.wheelRPM(60);
  leftWheel.wheelRPM(60);
}

void go() {
  desiredSpeed = 40;
  currentSpeed = currentSpeed + round((desiredSpeed - currentSpeed) * 0.1);
}

void goForwardFull () {
  Serial.println("goForwardFull");
  Serial.println(currentSpeed);
  goFull();
  setLeftWheel(currentSpeed);
  setRightWheel(currentSpeed);
}

void goLeft () {
  Serial.println("goLeft");
  go();
  setLeftWheel(0);
  setRightWheel(currentSpeed);
}

void goRight () {
  Serial.println("goRight");
  go();
  setLeftWheel(0);
  setRightWheel(currentSpeed);
}

void goBack () {
  Serial.println("goBack");
  setLeftWheel(-100);
  setRightWheel(-100);
}


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

  Serial.println(LSS_DefaultBaud);
}

void loop() {
  // Move the LSS continuously in one direction

  trigSonicSensor(SS_LEFT_ID_TRIG);
  distanceLeft = readSonicSensor(SS_LEFT_ID_ECHO);
  trigSonicSensor(SS_CENT_ID_TRIG);
  distanceFront = readSonicSensor(SS_CENT_ID_ECHO);
  trigSonicSensor(SS_RIGH_ID_TRIG);
  distanceRight = readSonicSensor(SS_RIGH_ID_ECHO);

  distanceLeft = convertSonicSensorValueToCm(distanceLeft);
  distanceFront = convertSonicSensorValueToCm(distanceFront);
  distanceRight = convertSonicSensorValueToCm(distanceRight);

  Serial.print(distanceLeft);
  Serial.print(" ");
  Serial.print(distanceFront);
  Serial.print(" ");
  Serial.println(distanceRight);

  doTheThing();
  //goFull();
}

void doTheThing() {
    if ((distanceFront <= minFrontDistance) || (distanceLeft <= minSideDistance) || (distanceRight <= minSideDistance)) {
      if ((distanceLeft < stuckDistance) || (distanceRight < stuckDistance) || (distanceFront < stuckDistance)) {
        goBack();
        delay(1.5*delayTime);
      }
      else if ((distanceFront <= minFrontDistance) && (distanceLeft <= minSideDistance) && (distanceRight <= minSideDistance)) {
        goBack();
        delay(1.5*delayTime);
      }
      else if (distanceLeft > distanceRight ) {
        goLeft();
        delay(delayTime);
      }
      else if (distanceLeft <= distanceRight) {
        goRight();
        delay(delayTime);
      }
      else
        goForwardFull();
    }
    else
      goForwardFull(); 
}

int convertSonicSensorValueToCm(int sensorValue) {
  return (sensorValue/2) / 29.1;  
}

int readSonicSensor(int echoSonicSensorId) {
  int val = pulseIn(echoSonicSensorId, HIGH);
  if (val < 0) {
    val = 999;  
  }
  return val;
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
