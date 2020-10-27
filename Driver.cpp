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
  //invert one value
  rightWheel.wheelRPM(speeed);
}

void Driver::setLeftWheel(int speeed) {
  leftWheel.wheelRPM(speeed * -1);
}

void Driver::stopCar () {
  setRightWheel(0);
  setLeftWheel(0);
}

void Driver::goFull() {
  Serial.println("Go Full");
  rightWheel.wheelRPM(MAX_SPEED);
  leftWheel.wheelRPM(MAX_SPEED);
}

void Driver::go() {
  desiredSpeed = 40;
  currentSpeed = currentSpeed + round((desiredSpeed - currentSpeed) * 0.1);
}

void Driver::goForwardFull () {
  goFull();
  setLeftWheel(currentSpeed);
  setRightWheel(currentSpeed);
}

void Driver::goLeft () {
  Serial.println("goLeft");
  go();
  setLeftWheel(0);
  setRightWheel(currentSpeed);
}

void Driver::goRight () {
  Serial.println("goRight");
  go();
  setLeftWheel(0);
  setRightWheel(currentSpeed);
}

void Driver::goBack () {
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
