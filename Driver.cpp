/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/
#include "Arduino.h"
#include "Driver.h"
#include "config.h"

Driver::Driver() {}
Driver::Driver(SoftwareSerial &servoSerial) {
  // Initialize the LSS bus
  LSS::initBus(servoSerial, LSS_BAUD);
  servoSerial.begin(LSS_BAUD);
  desiredSpeed = 60;

  leftWheel = LSS(LSS_ID0);
  rightWheel = LSS(LSS_ID1);
}

void Driver::setRightWheel(int speeed) {
  rightWheel.wheelRPM(speeed);
}

void Driver::setLeftWheel(int speeed) {
    //invert one value
  leftWheel.wheelRPM(speeed * -1);
}

void Driver::stopCar () {
  state = "StopCar";
  Serial.println(state);
  setRightWheel(0);
  setLeftWheel(0);
}

void Driver::goFull() {
  state = "Go Full";
  Serial.print("Go Full: ");
  Serial.println(MAX_SPEED);
  
  rightWheel.wheelRPM(MAX_SPEED);
  leftWheel.wheelRPM(MAX_SPEED);
}

void Driver::go() {
  currentSpeed = 30;
}

void Driver::goForwardFull () {
  goFull();
  setLeftWheel(currentSpeed);
  setRightWheel(currentSpeed);
}

void Driver::goLeft () {
  state = "goLeft";
  Serial.println("goLeft");
  setLeftWheel(0);
  setRightWheel(30);
}

void Driver::goRight () {
  state = "goRight";
  Serial.println("goRight");
  setLeftWheel(30);
  setRightWheel(0);
}

void Driver::goBack () {
  state = "goBack";
  Serial.println("goBack");
  setLeftWheel(-100);
  setRightWheel(-100);
}

void Driver::drive() {
  if ((distanceFront <= minFrontDistance) || (distanceLeft <= minSideDistance) || (distanceRight <= minSideDistance)) {
    if ((distanceLeft < stuckDistance) || (distanceRight < stuckDistance) || (distanceFront < stuckDistance)) {
      goBack();
      delay(1.5 * delayTime);
    }
    else if ((distanceFront <= minFrontDistance) && (distanceLeft <= minSideDistance) && (distanceRight <= minSideDistance)) {
      goBack();
      delay(1.5 * delayTime);
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

void Driver::shake() {

  stopCar();
  delay(1000);
  for (int i = 0; i < 10; i++) {
    setRightWheel(MAX_SPEED);
    setLeftWheel(-MAX_SPEED);
    delay(50);
    setLeftWheel(-MAX_SPEED);
    setRightWheel(MAX_SPEED);
    delay(50);
  }
  stopCar();
  delay(1000);
}
