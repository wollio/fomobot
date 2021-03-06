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
  cutWheel = LSS(LSS_ID2);
  cutWheel.limp();
  
  Serial.println(LSS_ID3);
  headWheel = LSS(LSS_ID3);
  delay(1000);
  headWheel.move(0);
  delay(1000);
}

void Driver::headDown() {
  if (!isHeadDown) {
    isHeadDown = true;
    Serial.println("headDown");
    headWheel.moveRelative(-440);
    delay(1000);
  }
}

void Driver::headUp() {
  if (isHeadDown) {
    isHeadDown = false;
    Serial.println("headUp");
    headWheel.move(0);
    delay(1000);
  }
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

void Driver::cut() {
  Serial.println("cut the fuck");
  //myLSS.wheelRPM(60);
  cutWheel.moveRelative(3600);
  delay(5000);
  Serial.println("stop");
}

void Driver::drive() {

  if (random(0,9) > 8) {
    goRight();  
    Serial.println("forceRight");
    delay(1000);
  } else if (random(0,9) > 8) {
    goLeft();
    Serial.println("forceLeft");
    delay(1000);
  } else {


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

}
