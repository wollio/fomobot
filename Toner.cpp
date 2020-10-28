#include "Arduino.h"
#include "Toner.h"

Toner::Toner(){}
Toner::Toner(int pin) {
  tonerPin = pin;
  pinMode(pin, OUTPUT);
  randomSeed(analogRead(0));
}

void Toner::speak() {
  int K = 1000;
  switch (random(1, 7)) {
    case 1: phrase1(); break;
    case 2: phrase2(); break;
    case 3: phrase1(); phrase2(); break;
    case 4: phrase1(); phrase2(); phrase1(); break;
    case 5: phrase1(); phrase2(); phrase1(); phrase2(); phrase1(); break;
    case 6: phrase2(); phrase1(); phrase2(); break;
  }
  for (int i = 0; i <= random(3, 9); i++) {
    tone(tonerPin, K + random(-1700, 2000));
    delay(random(70, 170));
    noTone(tonerPin);
    delay(random(0, 30));
  }
  noTone(tonerPin);
  //delay(random(2000, 4000));
}

void Toner::phrase1() {
  int k = random(1000, 2000);

  for (int i = 0; i <=  random(100, 2000); i++) {
    tone(tonerPin, k + (-i * 2));
    delay(random(.9, 2));
  }
  for (int i = 0; i <= random(100, 1000); i++) {
    tone(tonerPin, k + (i * 10));
    delay(random(.9, 2));
  }
}

void Toner::phrase2() {
  int k = random(1000, 2000);
  for (int i = 0; i <= random(100, 2000); i++) {
    tone(tonerPin, k + (i * 2));
    delay(random(.9, 2));
  }
  for (int i = 0; i <= random(100, 1000); i++) {
    tone(tonerPin, k + (-i * 10));
    delay(random(.9, 2));
  }
}
