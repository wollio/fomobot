#ifndef Driver_h
#define Driver_h

#include "Arduino.h"
#include <LSS.h>

const int minFrontDistance = 20;
const int minSideDistance = 25;
const int stuckDistance = 10;
const int delayTime = 150;
const bool isHeadDown = false;

class Driver
{
  public:
    Driver::Driver();
    Driver::Driver(SoftwareSerial &servoSerial);
    int distanceRight;
    int distanceLeft;
    int distanceFront;
    bool isHeadDown;
    void stopCar();
    void drive();
    void shake();
    void cut();
    void headDown();
    void headUp();
    String state;
  private:
    int currentSpeed;
    int desiredSpeed;
    int headAngle;
    LSS leftWheel;
    LSS rightWheel;
    LSS cutWheel;
    LSS headWheel;
    void setRightWheel(int speeed);
    void setLeftWheel(int speeed);
    void go();
    void goFull();
    void goForwardFull();
    void goLeft();
    void goRight();
    void goBack();
};

#endif
