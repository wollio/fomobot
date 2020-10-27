#ifndef Driver_h
#define Driver_h

#include "Arduino.h"
#include <LSS.h>

const int minFrontDistance = 15;
const int minSideDistance = 10;
const int stuckDistance = 5;
const int delayTime = 150;

class Driver
{
  public:
    Driver::Driver();
    Driver::Driver(SoftwareSerial &servoSerial);
    int distanceRight;
    int distanceLeft;
    int distanceFront;
    void drive();
  private:
    int currentSpeed;
    int desiredSpeed;
    LSS leftWheel;
    LSS rightWheel;
    void setRightWheel(int speeed);
    void setLeftWheel(int speeed);
    void stopCar();
    void go();
    void goFull();
    void goForwardFull();
    void goLeft();
    void goRight();
    void goBack();
};

#endif
