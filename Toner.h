#ifndef Toner_h
#define Toner_h

#include "Arduino.h"

class Toner
{
  public:
    Toner::Toner();
    Toner::Toner(int pin);
    int tonerPin;
    void speak();
  private:
    void phrase1();
    void phrase2();
};

#endif
